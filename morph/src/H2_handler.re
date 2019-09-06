open Morph_core;

let error_handler = (_client_address, ~request as _=?, _error, start_response) => {
  open H2;
  let response_body = start_response(Headers.empty);
  Body.close_writer(response_body);
};

let route_handler:
  (
    Opium_core.Service.t(Request.t, Response.t),
    list(Opium_core.Filter.simple(Request.t, Response.t)),
    Unix.sockaddr,
    H2.Reqd.t
  ) =>
  unit =
  (handler, middlewares, _client_address, request_descriptor) => {
    Lwt.async(() => {
      let H2.{Request.target, meth, headers, scheme: _} =
        H2.Reqd.request(request_descriptor);

      let content_length =
        H2.Headers.get(headers, "content-length")
        |> Utils.map_or(~default=128, int_of_string);

      let read_body = () =>
        Body.read(
          ~content_length,
          ~get_request_body=H2.Reqd.request_body,
          ~schedule_read=H2.Body.schedule_read,
          request_descriptor,
        );

      let create_response = (~headers, status) =>
        H2.Response.create(~headers, status);

      let request =
        Request.{
          target,
          meth,
          headers: H2.Headers.to_list(headers),
          read_body,
          context: Hmap.empty,
        };

      let handler = Opium_core.Filter.apply_all(middlewares, handler);

      handler(request)
      |> Lwt.map((response: Response.t) => {
           let headers = H2.Headers.of_list(response.headers);

           let () =
             H2.Reqd.respond_with_string(
               request_descriptor,
               create_response(~headers, response.status),
               response.body,
             );
           ();
         });
    });
  };
