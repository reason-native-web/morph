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
    Morph.Server.handler(
      [ | `String(string)],
      [
        | `String(string)
        | `Stream(Lwt_stream.t(char))
        | `StringStream(Lwt_stream.t(string))
      ],
    ),
    Unix.sockaddr,
    H2.Reqd.t
  ) =>
  unit;
