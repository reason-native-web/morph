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
    let read_stream = () => {
      Lwt_stream.iter(body => {write_char(response_body, body)}, stream);
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
