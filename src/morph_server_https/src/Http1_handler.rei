let error_handler:
  (
    Unix.sockaddr,
    ~request: 'request=?,
    'error,
    Httpaf.Headers.t => Httpaf.Body.t([ | `write])
  ) =>
  unit;

let make:
  (
    Opium_core.Service.t(Morph.Request.t, Morph.Response.t),
    Unix.sockaddr,
    Httpaf.Reqd.t('reqd)
  ) =>
  unit;
