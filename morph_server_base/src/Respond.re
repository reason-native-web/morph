exception Incorrect_Body;

let respond =
    (
      ~http_response,
      ~respond_with_string,
      ~respond_with_streaming,
      ~write_string,
      ~write_char,
      ~close_writer,
      ~flush_body,
      request_descriptor,
      response: Morph.Response.success,
    ) => {
  switch (response.body) {
  | Morph.Response.String(body) =>
    respond_with_string(request_descriptor, http_response, body)
  | Morph_base.Response.Stream(stream) =>
    let response_body =
      respond_with_streaming(request_descriptor, http_response);
    let read_stream = () => {
      let _ =
        Lwt_stream.iter(body => write_char(response_body, body), stream);
      ();
    };
    let _ =
      Lwt.bind(Lwt_stream.closed(stream), _ => {
        flush_body(response_body, () => close_writer(response_body))
        |> Lwt.return
      });
    read_stream();
  | Morph_base.Response.StringStream(stream) =>
    let response_body =
      respond_with_streaming(request_descriptor, http_response);
    let read_stream = () => {
      let _ =
        Lwt_stream.iter(
          body => {
            write_string(response_body, body);
            flush_body(response_body, () => ());
          },
          stream,
        );
      ();
    };
    let _ =
      Lwt.bind(Lwt_stream.closed(stream), _ => {
        flush_body(response_body, () => close_writer(response_body))
        |> Lwt.return
      });
    read_stream();
  | _ => raise(Incorrect_Body)
  };
};
