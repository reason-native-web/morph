type route = Routes.route(Server.handler);

/**
[make] takes lists of routes for each method and returns a Morph.Server.handler

[print] will defaults to true which will print the routing table

{2 Sample usage:}
{[
let get_routes =
  Routes.[
    s("greet") /? nil @--> greet_handler,
    s("set_greeting") / str /? nil @--> set_greeting_handler,
    s("secure") /? nil @--> secured_handler,
  ];

let handler = Morph.Router.make(~get=get_routes, ());

let server =
  Morph.Server.make(~port=3333, ~address=Unix.inet_addr_loopback, ());

server.start(handler) |> Lwt_main.run
]}

Output:
{[
Routing table

GET
/greet
/secure
/set_greeting/:string
]}
*/
let make:
  (
    ~get: list(route)=?,
    ~post: list(route)=?,
    ~put: list(route)=?,
    ~del: list(route)=?,
    ~print: bool=?,
    ~not_found_handler: Server.handler=?,
    unit
  ) =>
  Server.handler;
