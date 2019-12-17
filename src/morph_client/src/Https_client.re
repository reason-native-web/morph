let make =
    (
      ~socket,
      ~url_host,
      ~response_handler,
      ~error_handler,
      ~headers,
      ~meth,
      ~body,
      url_path,
    ) => {
  open Lwt.Infix;

  let client =
    Lwt_ssl.embed_uninitialized_socket(
      socket,
      Ssl.create_context(Ssl.TLSv1_2, Ssl.Client_context),
    );

  let () =
    Ssl.set_client_SNI_hostname(
      Lwt_ssl.ssl_socket_of_uninitialized_socket(client),
      url_host,
    );

  Lwt_ssl.ssl_perform_handshake(client)
  >>= Httpaf_lwt_unix.Client.SSL.create_connection
  >|= (
    connection => {
      let request_body =
        Httpaf_lwt_unix.Client.SSL.request(
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
  );
};
