let map_or = (~default: 'b, fn: 'a => 'b, opt: option('a)) =>
  switch (opt) {
  | Some(value) => fn(value)
  | None => default
  };

let error_handler = (_client_address, ~request as _=?, _error, start_response) => {
  let response_body = start_response(Httpaf.Headers.empty);
  Httpaf.Body.close_writer(response_body);
};

let make =
    (
      handler: Morph.Server.handler(string),
      _client_address: Unix.sockaddr,
      request_descriptor: Httpaf.Reqd.t('handle, 'io),
    ) => {
  Lwt.async(() => {
    let Httpaf.{Request.target, meth, headers, version: _} =
      Httpaf.Reqd.request(request_descriptor);

    let content_length =
      Httpaf.Headers.get(headers, "content-length")
      |> map_or(~default=128, int_of_string);

    let read_body = () =>
      Morph_server_base.Body.read(
        ~content_length,
        ~get_request_body=Httpaf.Reqd.request_body,
        ~schedule_read=Httpaf.Body.schedule_read,
        request_descriptor,
      );

    let create_response = (~headers, status) =>
      Httpaf.Response.create(~headers, ~reason=?None, ~version=?None, status);

    Lwt.bind(read_body(), (body: string) =>
      handler(
        Morph.Request.{
          target,
          meth,
          headers: Httpaf.Headers.to_list(headers),
          body,
          context: Hmap.empty,
        },
      )
    )
    |> Lwt.map((response: Morph.Response.t) => {
         let response =
           switch (response) {
           | Error(failure) => Morph.Response.success_of_failure(failure)
           | Ok(response) => response
           };

         let headers = Httpaf.Headers.of_list(response.headers);

         Morph_server_base.Respond.respond(
           ~http_response=
             create_response(~headers, Status.to_httpaf(response.status)),
           ~respond_with_string=Httpaf.Reqd.respond_with_string,
           ~respond_with_streaming=
             Httpaf.Reqd.respond_with_streaming(
               ~flush_headers_immediately=true,
             ),
           ~write_string=Httpaf.Body.write_string(~off=?None, ~len=?None),
           ~write_char=Httpaf.Body.write_char,
           ~close_writer=Httpaf.Body.close_writer,
           ~flush_body=Httpaf.Body.flush,
           request_descriptor,
           response,
         );
       });
  });
};
