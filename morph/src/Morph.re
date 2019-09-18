let h2_handler = (~request_handler, ssl_server) =>
  H2_lwt_unix.Server.SSL.create_connection_handler(
    ~certfile=?None,
    ~keyfile=?None,
    ~config=?None,
    ~server=ssl_server,
    ~request_handler,
    ~error_handler=H2_handler.error_handler,
  );

let http1_handler = (~request_handler, ssl_server) =>
  Httpaf_lwt_unix.Server.SSL.create_connection_handler(
    ~certfile=?None,
    ~keyfile=?None,
    ~config=?None,
    ~server=ssl_server,
    ~request_handler,
    ~error_handler=Http1_handler.error_handler,
  );

let startHttpServer = (~port=8080, ~middlewares, handler) => {
  open Lwt.Infix;

  let listen_address = Unix.(ADDR_INET(inet_addr_loopback, port));

  Lwt.async(() => {
    let connection_handler =
      Httpaf_lwt_unix.Server.create_connection_handler(
        ~config=?None,
        ~request_handler=Http1_handler.route_handler(handler, middlewares),
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

let rec first_match = l1 =>
  fun
  | [] => None
  | [x, ..._] when List.mem(x, l1) => Some(x)
  | [_, ...xs] => first_match(l1, xs);

let startHttpsServer = (~port=9443, ~cert, ~priv_key, ~middlewares, handler) => {
  open Lwt.Infix;

  let listen_address = Unix.(ADDR_INET(inet_addr_loopback, port));

  Lwt.async(() => {
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
                    ~request_handler=
                      Http1_handler.route_handler(handler, middlewares),
                    ssl_server,
                    client_addr,
                    fd,
                  )
                | Some("h2") =>
                  h2_handler(
                    ~request_handler=
                      H2_handler.route_handler(handler, middlewares),
                    ssl_server,
                    client_addr,
                    fd,
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

type handler =
  Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t);

type middleware =
  Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t);

let start_server =
    (
      ~http_port=8080,
      ~https_port=9443,
      ~cert=?,
      ~priv_key=?,
      ~middlewares=[],
      handler,
    ) => {
  if (Sys.unix) {
    Sys.(set_signal(sigpipe, Signal_handle(_ => ())));
  };

  switch (cert, priv_key) {
  | (Some(cert), Some(priv_key)) =>
    Lwt.join([
      startHttpServer(~port=http_port, ~middlewares, handler),
      startHttpsServer(
        ~port=https_port,
        ~cert,
        ~priv_key,
        ~middlewares,
        handler,
      ),
    ])
  | _ => startHttpServer(~port=http_port, ~middlewares, handler)
  };
};
