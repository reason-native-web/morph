module Response: {
  type Morph.Response.body +=
    | Stream(Lwt_stream.t(char))
    | StringStream(Lwt_stream.t(string));

  let byte_stream:
    (~stream: Lwt_stream.t(char), Morph.Response.t) =>
    Lwt.t(Morph.Response.t);

  let string_stream:
    (~stream: Lwt_stream.t(string), Morph.Response.t) =>
    Lwt.t(Morph.Response.t);

  /**
    [static file_path response] is a conventience function to return a file.
    */
  let static:
    (~file_path: string, Morph.Response.t) => Lwt.t(Morph.Response.t);
};
