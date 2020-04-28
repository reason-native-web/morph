/**
A [handler] takes a [Request.t] and returns a [Response.t] wrapepd in a [Lwt.t]
*/
type handler = Piaf.Server.Service.t(Request.t, Response.t);

/**
A [Server.t] is a record that describes a server.
*/
type t = {
  start: handler => Lwt.t(unit),
  port: int,
};

type middleware = handler => handler;

/**
Apply a list of middlewares to a handler
*/
let apply_all: (list(handler => handler), handler) => handler;

/**
Make the default server
*/

let make: (~port: int=?, ~address: Unix.inet_addr=?, unit) => t;
