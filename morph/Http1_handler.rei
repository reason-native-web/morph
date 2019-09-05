let error_handler:
  (
    Unix.sockaddr,
    ~request: 'request=?,
    'error,
    Httpaf.Headers.t => Httpaf.Body.t([ | `write])
  ) =>
  unit;

let route_handler:
  (
    Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t),
    list(
      Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t),
    ),
    Unix.sockaddr,
    Httpaf.Reqd.t('reqd)
  ) =>
  unit;
