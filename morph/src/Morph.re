let start =
    (
      ~servers: list(Server.t),
      ~middlewares: list(Server.middleware)=[],
      handler: Server.handler,
    ) => {
  if (Sys.unix) {
    Sys.(set_signal(sigpipe, Signal_handle(_ => ())));
  };

  let handler = Server.apply_all(middlewares, handler);

  servers
  |> List.map((server: Server.t) => server.start(handler))
  |> Lwt.join;
};

module Server = Server;

module Request = Request;
module Response = Response;

module Session = MSession;
