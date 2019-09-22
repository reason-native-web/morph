let respond =
    (
      ~http_response,
      ~respond_with_string,
      ~respond_with_streaming,
      ~write_char,
      ~close_writer,
      request_descriptor,
      response: Morph.Response.t,
    ) => {
  switch (response.body) {
  | `String(body) =>
    let () = respond_with_string(request_descriptor, http_response, body);
    ();
  | `Stream(stream) =>
    let response_body =
      respond_with_streaming(request_descriptor, http_response);
    let rec read_stream = () => {
      Lwt.bind(Lwt_stream.get(stream), maybe_body =>
        switch (maybe_body) {
        | Some(body) =>
          write_char(response_body, body);
          read_stream();
        | None => Lwt.return_unit
        }
      );
    };
    let _ =
      Lwt.bind(
        Lwt_stream.closed(stream),
        _ => {
          close_writer(response_body);
          Lwt.return_unit;
        },
      );
    let _ = read_stream();
    ();
  };
};
