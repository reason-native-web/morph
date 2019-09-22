type handler =
  Opium_core.Service.t(Request.t, Response.t);

type middleware =
  Opium_core.Filter.simple(Request.t, Response.t);

type t = {
  start: handler => Lwt.t(unit),
  port: int,
};
