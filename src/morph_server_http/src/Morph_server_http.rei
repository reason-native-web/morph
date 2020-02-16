let make:
  (~port: int=?, ~address: Unix.inet_addr=?, unit) =>
  Morph.Server.t(
    [ | `String(string)],
    [
      | `String(string)
      | `Stream(Lwt_stream.t(char))
      | `StringStream(Lwt_stream.t(string))
    ],
  );
