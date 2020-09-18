let setup_log = (~style_renderer=?, level) => {
  Fmt_tty.setup_std_outputs(~style_renderer?, ());
  Logs.set_level(Some(level));
  Logs.set_reporter(Logs_fmt.reporter());
};

let server =
  Morph.Server.make(~port=3333, ~address=Unix.inet_addr_loopback, ());

let greet_session_key = "__greet__";

let set_session_handler = (value, request) => {
  Morph.Middlewares.Session.set(request, ~key=greet_session_key, ~value)
  |> Lwt.map(Morph.Response.ok);
};

let greet_handler: Morph.Server.handler =
  request => {
    Morph.Middlewares.Session.get(request, ~key=greet_session_key)
    |> Lwt.map(
         fun
         | Ok(session) => Morph.Response.text("Welcome " ++ session)
         | Error(_) => Error(`User("no session found")),
       );
  };

let websocket_handler = request => {
  Morph_websocket.handler(
    (~send, ~reader, ~close as _) => {reader(send)},
    request,
  );
};
let graphql_handler = Morph_websocket.Graphql.request_handler(Schema.schema);

let grahiql_handler = _ => {
  Render.respond_html(GraphiQL.make());
};

let get_routes =
  Routes.[
    s("greet") /? nil @--> greet_handler,
    s("set_greeting") / str /? nil @--> set_session_handler,
    s("secure") /? nil @--> Secured.handler,
    s("ws") /? nil @--> websocket_handler,
    s("graphiql") /? nil @--> grahiql_handler,
    s("graphql") /? nil @--> graphql_handler,
  ];

let post_routes = Routes.[s("graphql") /? nil @--> graphql_handler];

let handler =
  Morph.Server.apply_all(
    [
      Morph.Middlewares.Static.make(~path="public", ~public_path="./bin"),
      Morph.Middlewares.Static.make(~path="docs", ~public_path="./_docs"),
      Morph.Middlewares.Session.middleware,
      Auth.middleware,
    ],
    Morph.Router.make(~get=get_routes, ~post=post_routes, ()),
  );

let () = {
  setup_log(Info);

  let () = Mirage_crypto_rng_unix.initialize();

  server.start(handler) |> Lwt_main.run;
};
