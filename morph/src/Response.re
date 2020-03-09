type headers = list((string, string));

type body = ..;

type body +=
  | String(string);

type success = {
  status: Status.t,
  headers,
  body,
};

type failure = [ | `User(string) | `Server(string) | `Auth(string)];

/**
The core [Response.t] type
*/
type t = result(success, failure);

let success_of_failure =
  fun
  | `User(message) => {
      status: `Bad_request,
      headers: [],
      body: String(message),
    }
  | `Server(message) => {
      status: `Internal_server_error,
      headers: [],
      body: String(message),
    }
  | `Auth(message) => {
      status: `Unauthorized,
      headers: [],
      body: String(message),
    };

let to_success =
  fun
  | Ok(success) => success
  | Error(failure) => success_of_failure(failure);

let result_map = (fn, res) => {
  switch (res) {
  | Ok(data) => Ok(fn(data))
  | e => e
  };
};

let empty = Ok({status: `OK, headers: [], body: String("")});

let make = (~status=`OK, ~headers=[], body) => Ok({status, headers, body});

let add_header = (new_header: (string, string)) => {
  result_map(res => {...res, headers: res.headers @ [new_header]});
};

let add_headers = (new_headers: headers) => {
  result_map(res => {...res, headers: res.headers @ new_headers});
};

let set_status = (status: Status.t) => {
  result_map(res => {...res, status});
};

let set_body = body => {
  result_map(res => {...res, body});
};

let ok = res => {
  add_header(("Content-length", "2"), res)
  |> set_status(`OK)
  |> set_body(String("ok"))
  |> Lwt.return;
};

let text = (text, res) => {
  let content_length = text |> String.length |> string_of_int;
  add_header(("Content-length", content_length), res)
  |> set_body(String(text))
  |> Lwt.return;
};

let json = (json, res) => {
  let content_length = json |> String.length |> string_of_int;
  add_header(("Content-type", "application/json"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(String(json))
  |> Lwt.return;
};

let html = (markup, res) => {
  let content_length = markup |> String.length |> string_of_int;
  add_header(("Content-type", "text/html"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(String(markup))
  |> Lwt.return;
};

let redirect = (~code=303, targetPath, res) => {
  let content_length = targetPath |> String.length |> string_of_int;

  add_header(("Content-length", content_length), res)
  |> add_header(("Location", targetPath))
  |> set_status(`Code(code))
  |> set_body(String(targetPath))
  |> Lwt.return;
};

let unauthorized = (message, res) => {
  add_header(
    ("Content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Unauthorized)
  |> set_body(String(message))
  |> Lwt.return;
};

let not_found = (~message="Not found", res) => {
  add_header(
    ("content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Not_found)
  |> set_body(String(message))
  |> Lwt.return;
};
