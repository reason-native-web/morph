let error_handler = (_client_address, ~request as _=?, _error, start_response) => {
  let response_body = start_response(Httpaf.Headers.empty);
  Httpaf.Body.close_writer(response_body);
};

let route_handler =
    (handler, middlewares, _client_address, request_descriptor) => {
  Lwt.async(() => {
    let Httpaf.{Request.target, meth, headers, version: _} =
      Httpaf.Reqd.request(request_descriptor);

    let content_length =
      Httpaf.Headers.get(headers, "content-length")
      |> Utils.map_or(~default=128, int_of_string);

    let read_body = () =>
      Body.read(
        ~content_length,
        ~get_request_body=Httpaf.Reqd.request_body,
        ~schedule_read=Httpaf.Body.schedule_read,
        request_descriptor,
      );

    let create_response = (~headers, status) =>
      Httpaf.Response.create(~headers, ~reason=?None, ~version=?None, status);

    let request =
      Morph_core.Request.{
        target,
        meth,
        headers: Httpaf.Headers.to_list(headers),
        read_body,
        context: Hmap.empty,
      };

    let handler = Opium_core.Filter.apply_all(middlewares, handler);

    handler(request)
    |> Lwt.map((response: Morph_core.Response.t) => {
         let headers = Httpaf.Headers.of_list(response.headers);

         Respond.respond(
           ~http_response=
             create_response(~headers, Status.to_httpaf(response.status)),
           ~respond_with_string=Httpaf.Reqd.respond_with_string,
           ~respond_with_streaming=
             Httpaf.Reqd.respond_with_streaming(
               ~flush_headers_immediately=true,
             ),
           ~write_char=Httpaf.Body.write_char,
           ~close_writer=Httpaf.Body.close_writer,
           request_descriptor,
           response,
         );
       });
  });
};
