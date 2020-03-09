let h2_handler = (~request_handler) =>
  H2_lwt_unix.Server.SSL.create_connection_handler(
    ~config=?None,
    ~error_handler=H2_handler.error_handler,
    ~request_handler,
  );

let http1_handler = (~request_handler) =>
  Httpaf_lwt_unix.Server.SSL.create_connection_handler(
    ~config=?None,
    ~request_handler,
    ~error_handler=Http1_handler.error_handler,
  );

let rec first_match = l1 =>
  fun
  | [] => None
  | [x, ..._] when List.mem(x, l1) => Some(x)
  | [_, ...xs] => first_match(l1, xs);

let make = (~port=9443, ~cert, ~priv_key, ()) => {
  let listen_address = Unix.(ADDR_INET(inet_addr_loopback, port));

  let start = handler => {
    Lwt.async(() => {
      open Lwt.Infix;
      let connection_handler = (client_addr, fd) =>
        Lwt.catch(
          () => {
            let server_ctx =
              Ssl.create_context(Ssl.TLSv1_3, Ssl.Server_context);

            Ssl.disable_protocols(server_ctx, [Ssl.SSLv23, Ssl.TLSv1_1]);
            Ssl.use_certificate(server_ctx, cert, priv_key);
            let protos = ["h2", "http/1.1"];
            Ssl.set_context_alpn_protos(server_ctx, protos);
            Ssl.set_context_alpn_select_callback(server_ctx, client_protos =>
              first_match(client_protos, protos)
            );
            Lwt_ssl.ssl_accept(fd, server_ctx)
            >>= (
              ssl_server =>
                switch (Lwt_ssl.ssl_socket(ssl_server)) {
                | None => assert(false)
                | Some(ssl_socket) =>
                  switch (Ssl.get_negotiated_alpn_protocol(ssl_socket)) {
                  | None =>
                    /* Unable to negotiate a protocol */
                    Lwt.return_unit
                  | Some("http/1.1") =>
                    http1_handler(
                      ~request_handler=Http1_handler.make(handler),
                      client_addr,
                      ssl_server,
                    )
                  | Some("h2") =>
                    h2_handler(
                      ~request_handler=H2_handler.make(handler),
                      client_addr,
                      ssl_server,
                    )
                  | Some(_) =>
                    /* Can't really happen - would mean that TLS negotiated a
                     * protocol that we didn't specify. */
                    assert(false)
                  }
                }
            );
          },
          exn => Lwt_io.eprintlf("EXN: %s", Printexc.to_string(exn)),
        );

      Lwt_io.establish_server_with_client_socket(
        listen_address,
        connection_handler,
      )
      >|= (
        _server => Logs.info(m => m("HTTPS server started on port: %d", port))
      );
    });

    let (forever, _) = Lwt.wait();
    forever;
  };
  Morph.Server.{start, port};
};
