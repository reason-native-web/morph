let make =
    (
      ~socket,
      ~url_host as _,
      ~response_handler,
      ~error_handler,
      ~headers,
      ~meth,
      ~body,
      url_path,
    ) => {
  Lwt.Infix.(
    Httpaf_lwt_unix.Client.create_connection(socket)
    >|= (
      connection => {
        let request_body =
          Httpaf_lwt_unix.Client.request(
            connection,
            ~error_handler,
            ~response_handler,
            Httpaf.Request.create(~headers, meth, url_path),
          );

        switch (body) {
        | "" => ()
        | body => Httpaf.Body.write_string(request_body, body)
        };

        Httpaf.Body.close_writer(request_body);
      }
    )
  );
};
