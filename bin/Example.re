Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Info));
Logs.set_reporter(Logs_fmt.reporter());

let entropy = Nocrypto_entropy_lwt.initialize();

let server =
  Morph.Server.make(~port=3333, ~address=Unix.inet_addr_loopback, ());

let greet_session_key = "__greet__";

let set_session_handler = (value, request) => {
  Morph.Session.set(request, ~key=greet_session_key, ~value)
  |> Lwt.map(Morph.Response.ok);
};

let greet_handler: Morph.Server.handler =
  request => {
    Morph.Session.get(request, ~key=greet_session_key)
    |> Lwt.map(
         fun
         | Ok(session) => Morph.Response.text("Welcome " ++ session)
         | Error(_) => Error(`User("no session found")),
       );
  };

let get_routes =
  Routes.[
    s("greet") /? nil @--> greet_handler,
    s("set") / str /? nil @--> set_session_handler,
    s("secure") /? nil @--> Secured.handler,
  ];

let handler = Morph.Server.apply_all(
  [Auth.middleware, Morph.Session.middleware],
  Router.make(~get=get_routes),
);

entropy |>
Lwt.map(() => server.start(handler))
|> Lwt_main.run;
