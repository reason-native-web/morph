let start =
    (
      ~servers: list(Server.t('req_body)),
      ~middlewares: list(Server.middleware('req_body))=[],
      handler: Server.handler('req_body),
    ) => {
  if (Sys.unix) {
    Sys.(set_signal(sigpipe, Signal_handle(_ => ())));
  };

  let handler = Server.apply_all(middlewares, handler);

  servers
  |> List.map((server: Server.t('req_body)) => server.start(handler))
  |> Lwt.join;
};

module Server = Server;

module Request = Request;
module Response = Response;

module Method = Method;
module Status = Status;
module Header = Header;
