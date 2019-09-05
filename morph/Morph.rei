/**
A [handler] takes a [Request.t] and returns a [Response.t] wrapepd in a [Lwt.t]

{[type handler = Request.t => Response.t]}

It's the same type as a [Opium_core.Service.t] parameterized with our [Request.t] and [Response.t].
*/
type handler =
  Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t);

/**
A [middleware] takes a [handler] and returns a [handler] wrapepd in a [Lwt.t] parameterized with our [Request.t] and [Response.t].

{[type middleware = service => service]}

It's the same type as a [Opium_core.Filter.t].
*/
type middleware =
  Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t);

/**
[start_server http_port https_port cert priv_key middlewares handler] starts the server.
If you provide [cert] and [priv_key] it will automatically start a https server with support fort HTTP/2

{2 Simple usage:}
{[
Morph.start_server(_request => Http.Response.Ok.make)
|> Lwt_main.run;
]}
*/
let start_server:
  (
    ~http_port: int=?,
    ~https_port: int=?,
    ~cert: string=?,
    ~priv_key: string=?,
    ~middlewares: list(middleware)=?,
    handler
  ) =>
  Lwt.t(unit);
