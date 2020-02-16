let make:
  (~port: int=?, unit) =>
  Morph.Server.t(
    [ | `String(string)],
    [
      | `String(string)
      | `Stream(Lwt_stream.t(char))
      | `StringStream(Lwt_stream.t(string))
    ],
  );
