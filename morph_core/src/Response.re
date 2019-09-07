type headers = list((string, string));

type body =
  | String(string)
  | Stream(Lwt_stream.t(string));

type t = {
  status: Status.t,
  headers,
  body,
};

let make = (~status=`OK, ~headers=[], body) => {status, headers, body};

let ok = (~extra_headers=[], ()) => {
  let headers = [("content-length", "2"), ...extra_headers];

  make(~status=`OK, ~headers, String("ok")) |> Lwt.return;
};

let text = (~extra_headers=[], text) => {
  let content_length = text |> String.length |> string_of_int;
  let headers = [("content-length", content_length), ...extra_headers];

  make(~status=`OK, ~headers, String(text)) |> Lwt.return;
};

let json = (~extra_headers=[], json) => {
  let content_length = json |> String.length |> string_of_int;
  let headers = [
    ("content-type", "application/json"),
    ("content-length", content_length),
    ...extra_headers,
  ];

  make(~status=`OK, ~headers, String(json)) |> Lwt.return;
};

let html = (~extra_headers=[], markup) => {
  let content_length = markup |> String.length |> string_of_int;
  let headers = [
    ("content-type", "text/html"),
    ("content-length", content_length),
    ...extra_headers,
  ];

  make(~status=`OK, ~headers, String(markup)) |> Lwt.return;
};

let redirect = (~extra_headers=?, ~code=303, targetPath) => {
  let content_length = targetPath |> String.length |> string_of_int;

  let constantHeaders = [
    ("content-length", content_length),
    ("location", targetPath),
  ];

  let headers =
    switch (extra_headers) {
    | Some(h) => List.concat([constantHeaders, h])
    | None => constantHeaders
    };

  make(~status=`Code(code), ~headers, String(targetPath)) |> Lwt.return;
};

let unauthorized = (~extra_headers=[], message) => {
  let headers = [
    ("content-length", String.length(message) |> string_of_int),
    ...extra_headers,
  ];

  make(~status=`Unauthorized, ~headers, String(message)) |> Lwt.return;
};

let not_found = (~extra_headers=[], ~message="Not found", ()) => {
  let headers = [
    ("content-length", String.length(message) |> string_of_int),
    ...extra_headers,
  ];

  make(~status=`Not_found, ~headers, String(message)) |> Lwt.return;
};

let get_file_stream = file_name => {
  let read_only_flags = [Unix.O_RDONLY];
  let read_only_perm = 444;
  let fd = Unix.openfile(file_name, read_only_flags, read_only_perm);
  Lwt_io.of_unix_fd(fd, ~mode=Lwt_io.Input) |> Lwt_io.read_lines;
};

let static = (~extra_headers=[], file_path) => {
  Sys.file_exists(file_path)
    ? {
      let headers = [
        ("content-type", MimeTypes.getMimeType(file_path)),
        ("connection", "close"),
        ...extra_headers,
      ];
      let stream = get_file_stream(file_path);
      make(~status=`OK, ~headers, Stream(stream)) |> Lwt.return;
    }
    : not_found(~extra_headers, ~message="File does not exist", ());
};