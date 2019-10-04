type headers = list((string, string));

type body = [
  | `String(string)
  | `Stream(Lwt_stream.t(char))
  | `StringStream(Lwt_stream.t(string))
];

type t = {
  status: Status.t,
  headers,
  body,
};

let empty = {status: `OK, headers: [], body: `String("")};

let make = (~status=`OK, ~headers=[], body) => {status, headers, body};

let add_header = (new_header: (string, string), res: t) => {
  {...res, headers: res.headers @ [new_header]};
};

let add_headers = (new_headers: headers, res: t) => {
  {...res, headers: res.headers @ new_headers};
};

let set_status = (status: Status.t, res: t) => {
  {...res, status};
};

let set_body = (body: body, res: t) => {
  {...res, body};
};

let ok = (res: t) => {
  add_header(("Content-length", "2"), res)
  |> set_status(`OK)
  |> set_body(`String("ok"))
  |> Lwt.return;
};

let text = (text, res: t) => {
  let content_length = text |> String.length |> string_of_int;
  add_header(("Content-length", content_length), res)
  |> set_body(`String(text))
  |> Lwt.return;
};

let json = (json, res: t) => {
  let content_length = json |> String.length |> string_of_int;
  add_header(("Content-type", "application/json"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(`String(json))
  |> Lwt.return;
};

let html = (markup, res: t) => {
  let content_length = markup |> String.length |> string_of_int;
  add_header(("Content-type", "text/html"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(`String(markup))
  |> Lwt.return;
};

let redirect = (~code=303, targetPath, res: t) => {
  let content_length = targetPath |> String.length |> string_of_int;

  add_header(("Content-length", content_length), res)
  |> add_header(("Location", targetPath))
  |> set_status(`Code(code))
  |> set_body(`String(targetPath))
  |> Lwt.return;
};

let unauthorized = (message, res: t) => {
  add_header(
    ("Content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Unauthorized)
  |> set_body(`String(message))
  |> Lwt.return;
};

let not_found = (~message="Not found", res: t) => {
  add_header(
    ("content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Not_found)
  |> set_body(`String(message))
  |> Lwt.return;
};

let get_file_stream = file_name => {
  let read_only_flags = [Unix.O_RDONLY];
  let read_only_perm = 444;
  let fd = Unix.openfile(file_name, read_only_flags, read_only_perm);
  Lwt_io.of_unix_fd(fd, ~mode=Lwt_io.Input) |> Lwt_io.read_chars;
};

let static = (file_path, res: t) => {
  Sys.file_exists(file_path)
    ? {
      let size = Unix.stat(file_path).st_size;
      let stream = get_file_stream(file_path);
      add_header(("Content-type", Magic_mime.lookup(file_path)), res)
      |> add_header(("Content-length", string_of_int(size)))
      |> set_body(`Stream(stream))
      |> Lwt.return;
    }
    : not_found(~message="File does not exist", res);
};
