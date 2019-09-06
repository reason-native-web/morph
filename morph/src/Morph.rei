/**
A [handler] takes a [Request.t] and returns a [Response.t] wrapepd in a [Lwt.t]

See {{: https://rgrinberg.github.io/opium/opium_core/index.html#services} [Opium_core.Service.t]} for documentation on Services.
*/
type handler =
  Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t);

/**
A [middleware] takes a [handler] and returns a [handler].

See {{: https://rgrinberg.github.io/opium/opium_core/index.html#filters} [Opium_core.Filter.t]} for documentation on Filters.
*/
type middleware =
  Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t);

/**
[start_server http_port https_port cert priv_key middlewares handler] starts the server.

[http_port] defaults to 8080

[https_port] defaults to 9443

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
