/**
A [handler] takes a [Request.t] and returns a [Response.t] wrapepd in a [Lwt.t]

See {{: https://rgrinberg.github.io/opium/opium_core/index.html#services} [Opium_core.Service.t]} for documentation on Services.
*/
type handler = Opium_core.Service.t(Request.t, Response.t);

/**
A [middleware] takes a [handler] and returns a [handler].

See {{: https://rgrinberg.github.io/opium/opium_core/index.html#filters} [Opium_core.Filter.t]} for documentation on Filters.
*/
type middleware = Opium_core.Filter.simple(Request.t, Response.t);

/**
A [Server.t] is a record that describes a server.
*/
type t = {
  start: handler => Lwt.t(unit),
  port: int,
};
