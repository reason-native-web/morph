let error_handler:
  (
    Unix.sockaddr,
    ~request: 'request=?,
    'error,
    H2.Headers.t => H2.Body.t([ | `write])
  ) =>
  unit;

let make:
  (Morph.Server.handler(string), Unix.sockaddr, H2.Reqd.t) =>
  unit;
