let error_handler:
  (
    Unix.sockaddr,
    ~request: 'request=?,
    'error,
    H2.Headers.t => H2.Body.t([ | `write])
  ) =>
  unit;

let route_handler:
  (
    Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t),
    list(
      Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t),
    ),
    Unix.sockaddr,
    H2.Reqd.t
  ) =>
  unit;
