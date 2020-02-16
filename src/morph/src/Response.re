type headers = list((string, string));

/*
 type body = [>
   | `String(string)
 ];
 */

type body('res_body) = [> | `String(string)] as 'res_body;

type success('res_body) = {
  status: Status.t,
  headers,
  body: body('res_body),
};

type failure = [ | `User(string) | `Server(string) | `Auth(string)];

/**
The core [Response.t] type
*/
type t('res_body) = result(success('res_body), failure);

let success_of_failure =
  fun
  | `User(message) => {
      status: `Bad_request,
      headers: [],
      body: `String(message),
    }
  | `Server(message) => {
      status: `Internal_server_error,
      headers: [],
      body: `String(message),
    }
  | `Auth(message) => {
      status: `Unauthorized,
      headers: [],
      body: `String(message),
    };

let result_map = (fn, res) => {
  switch (res) {
  | Ok(data) => Ok(fn(data))
  | e => e
  };
};

let empty = Ok({status: `OK, headers: [], body: `String("")});

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
  |> set_body(`String("ok"));
};

let text = (text, res) => {
  let content_length = text |> String.length |> string_of_int;
  add_header(("Content-length", content_length), res)
  |> set_body(`String(text));
};

let json = (json, res) => {
  let content_length = json |> String.length |> string_of_int;
  add_header(("Content-type", "application/json"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(`String(json));
};

let html = (markup, res) => {
  let content_length = markup |> String.length |> string_of_int;
  add_header(("Content-type", "text/html"), res)
  |> add_header(("Content-length", content_length))
  |> set_body(`String(markup));
};

let redirect = (~code=303, targetPath, res) => {
  let content_length = targetPath |> String.length |> string_of_int;

  add_header(("Content-length", content_length), res)
  |> add_header(("Location", targetPath))
  |> set_status(`Code(code))
  |> set_body(`String(targetPath));
};

let unauthorized = (message, res) => {
  add_header(
    ("Content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Unauthorized)
  |> set_body(`String(message));
};

let not_found = (~message="Not found", res) => {
  add_header(
    ("content-length", String.length(message) |> string_of_int),
    res,
  )
  |> set_status(`Not_found)
  |> set_body(`String(message));
};
/*
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
 */
