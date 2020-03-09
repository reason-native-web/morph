type Morph.Response.body +=
  | Stream(Lwt_stream.t(char))
  | StringStream(Lwt_stream.t(string));

let byte_stream = (~stream, res) => {
  Morph.Response.set_body(Stream(stream), res) |> Lwt.return;
};

let string_stream = (~stream, res) => {
  Morph.Response.set_body(StringStream(stream), res) |> Lwt.return;
};

let get_file_stream = file_name => {
  let read_only_flags = [Unix.O_RDONLY];
  let read_only_perm = 444;
  let fd = Unix.openfile(file_name, read_only_flags, read_only_perm);
  Lwt_io.of_unix_fd(fd, ~mode=Lwt_io.Input) |> Lwt_io.read_chars;
};

let static = (~file_path, res) => {
  Sys.file_exists(file_path)
    ? {
      let size = Unix.stat(file_path).st_size;
      let stream = get_file_stream(file_path);
      Morph.Response.add_header(
        ("Content-type", Magic_mime.lookup(file_path)),
        res,
      )
      |> Morph.Response.add_header(("Content-length", string_of_int(size)))
      |> byte_stream(~stream);
    }
    : Morph.Response.not_found(~message="File does not exist", res);
};
