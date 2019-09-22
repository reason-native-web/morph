let error_handler:
  (
    Unix.sockaddr,
    ~request: 'request=?,
    'error,
    H2.Headers.t => H2.Body.t([ | `write])
  ) =>
  unit;

let make:
  (
    Opium_core.Service.t(Morph.Request.t, Morph.Response.t),
    Unix.sockaddr,
    H2.Reqd.t
  ) =>
  unit;
