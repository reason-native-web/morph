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
    Morph.Server.handler(
      [ | `String(string)],
      [
        | `String(string)
        | `Stream(Lwt_stream.t(char))
        | `StringStream(Lwt_stream.t(string))
      ],
    ),
    Unix.sockaddr,
    Httpaf.Reqd.t('handle, 'io)
  ) =>
  unit;
