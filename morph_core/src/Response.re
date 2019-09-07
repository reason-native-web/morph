type headers = list((string, string));

type t = {
  status: Status.t,
  headers,
  body: string,
};

let make = (~status=`OK, ~headers=[], body) => {status, headers, body};

let ok = (~extra_headers=[], ()) => {
  let headers = [("content-length", "2"), ...extra_headers];

  make(~status=`OK, ~headers, "ok") |> Lwt.return;
};

let text = (~extra_headers=[], text) => {
  let content_length = text |> String.length |> string_of_int;
  let headers = [("content-length", content_length), ...extra_headers];

  make(~status=`OK, ~headers, text) |> Lwt.return;
};

let json = (~extra_headers=[], json) => {
  let content_length = json |> String.length |> string_of_int;
  let headers = [
    ("content-type", "application/json"),
    ("content-length", content_length),
    ...extra_headers,
  ];

  make(~status=`OK, ~headers, json) |> Lwt.return;
};

let html = (~extra_headers=[], markup) => {
  let content_length = markup |> String.length |> string_of_int;
  let headers = [
    ("content-type", "text/html"),
    ("content-length", content_length),
    ...extra_headers,
  ];

  make(~status=`OK, ~headers, markup) |> Lwt.return;
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

  make(~status=`Code(code), ~headers, targetPath) |> Lwt.return;
};

let unauthorized = (~extra_headers=[], message) => {
  let headers = [
    ("content-length", String.length(message) |> string_of_int),
    ...extra_headers,
  ];

  make(~status=`Unauthorized, ~headers, message) |> Lwt.return;
};

let not_found = (~extra_headers=[], ~message="Not found", ()) => {
  let headers = [
    ("content-length", String.length(message) |> string_of_int),
    ...extra_headers,
  ];

  make(~status=`Not_found, ~headers, message) |> Lwt.return;
};
