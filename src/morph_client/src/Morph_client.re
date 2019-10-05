open Lwt.Infix;

external from_httpaf_status: Httpaf.Status.t => Morph.Status.t = "%identity";

let error_handler = _ => assert(false);

let flat_map_opt = (fn, opt) => {
  switch (opt) {
  | Some(x) => Some(fn(x))
  | None => None
  };
};

let get_or_opt = (~default, opt) => {
  switch (opt) {
  | Some(x) => x
  | None => default
  };
};

let read_response = (~notify_finished, response, response_body) => {
  switch (response) {
  | {Httpaf.Response.status: `OK, headers, _} =>
    let content_length =
      Httpaf.Headers.get(headers, "content-length")
      |> flat_map_opt(int_of_string)
      |> get_or_opt(~default=2048);

    let body_buffer = Buffer.create(content_length);

    let rec on_read = (bs, ~off, ~len) => {
      let request_data_bytes = Bytes.create(len);
      Bigstringaf.blit_to_bytes(
        bs,
        ~src_off=0,
        request_data_bytes,
        ~dst_off=off,
        ~len,
      );
      Buffer.add_bytes(body_buffer, request_data_bytes);
      Httpaf.Body.schedule_read(response_body, ~on_read, ~on_eof);
    }
    and on_eof = () => {
      Lwt.wakeup_later(
        notify_finished,
        Morph.Response.{
          status: `OK,
          body: `String(Buffer.contents(body_buffer)),
          headers: Httpaf.Headers.to_list(headers),
        },
      );
    };

    Httpaf.Body.schedule_read(response_body, ~on_read, ~on_eof);
  | {headers, status, _} =>
    Logs.err(m => m("%a\n%!", Httpaf.Response.pp_hum, response));
    let content_length =
      Httpaf.Headers.get(headers, "content-length")
      |> flat_map_opt(int_of_string)
      |> get_or_opt(~default=2048);

    let body_buffer = Buffer.create(content_length);

    let rec on_read = (bs, ~off, ~len) => {
      let request_data_bytes = Bytes.create(len);
      Bigstringaf.blit_to_bytes(
        bs,
        ~src_off=0,
        request_data_bytes,
        ~dst_off=off,
        ~len,
      );
      Buffer.add_bytes(body_buffer, request_data_bytes);
      Httpaf.Body.schedule_read(response_body, ~on_read, ~on_eof);
    }
    and on_eof = () => {
      Lwt.wakeup_later(
        notify_finished,
        Morph.Response.{
          status: from_httpaf_status(status),
          body: `String(Buffer.contents(body_buffer)),
          headers: Httpaf.Headers.to_list(headers),
        },
      );
    };

    Httpaf.Body.schedule_read(response_body, ~on_read, ~on_eof);
  };
};

let handle = (request: Morph.Request.t): Lwt.t(Morph.Response.t) => {
  let Morph.Request.{target, meth, headers, read_body, _} = request;

  let uri = Uri.of_string(target);

  let url_host = Uri.host_with_default(~default="", uri);
  let url_scheme =
    Uri.scheme(uri)
    |> (
      fun
      | Some(scheme) => scheme
      | None => "https"
    );
  let url_port =
    Uri.port(uri)
    |> (
      fun
      | Some(port) => string_of_int(port)
      | None when url_scheme == "https" => "443"
      | None => "80"
    );
  let url_path = Uri.path(uri);

  Lwt_unix.getaddrinfo(url_host, url_port, [Unix.(AI_FAMILY(PF_INET))])
  >>= (
    addresses => {
      let socket = Lwt_unix.socket(Unix.PF_INET, Unix.SOCK_STREAM, 0);

      Lwt_unix.connect(socket, List.hd(addresses).Unix.ai_addr)
      >>= read_body
      >>= (
        body => {
          let (finished, notify_finished) = Lwt.wait();

          let response_handler = read_response(~notify_finished);

          let headers =
            switch (body) {
            | "" => headers
            | body => [
                ("Content-Length", string_of_int(String.length(body))),
                ...headers,
              ]
            };

          let headers =
            Httpaf.Headers.of_list([
              ("Connection", "close"),
              ("Host", url_host),
              ...headers,
            ]);

          (
            if (url_scheme == "https") {
              Https_client.make(
                ~socket,
                ~url_host,
                ~response_handler,
                ~error_handler,
                ~headers,
                ~meth,
                ~body,
                url_path,
              );
            } else {
              Http_client.make(
                ~socket,
                ~url_host,
                ~response_handler,
                ~error_handler,
                ~headers,
                ~meth,
                ~body,
                url_path,
              );
            }
          )
          >>= (() => finished);
        }
      );
    }
  );
};
