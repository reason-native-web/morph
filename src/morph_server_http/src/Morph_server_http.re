let make = (~port=8080, ()) => {
  let listen_address = Unix.(ADDR_INET(inet_addr_any, port));
  let start = handler => {
    Lwt.async(() => {
      open Lwt.Infix;

      let connection_handler =
        Httpaf_lwt_unix.Server.create_connection_handler(
          ~config=?None,
          ~request_handler=Http1_handler.make(handler),
          ~error_handler=Http1_handler.error_handler,
        );

      Lwt_io.establish_server_with_client_socket(
        listen_address,
        connection_handler,
      )
      >|= (
        _server => Logs.info(m => m("HTTP server started on port: %d", port))
      );
    });

    let (forever, _) = Lwt.wait();
    forever;
  };
  Morph.Server.{start, port};
};
