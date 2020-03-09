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
    Morph.Server.handler(string),
    Unix.sockaddr,
    Httpaf.Reqd.t('handle, 'io)
  ) =>
  unit;
