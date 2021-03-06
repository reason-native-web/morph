type body = Piaf.Body.t;

type failure = [
  | `User(string)
  | `Server(string)
  | `Auth(string)
  | Piaf.Error.t
];

/**
The core [Response.t] type
*/
type t = result(Piaf.Response.t, failure);

let response_of_failure =
  fun
  | `User(message) =>
    Piaf.Response.create(~body=Piaf.Body.of_string(message), `Bad_request)
  | `Server(message) =>
    Piaf.Response.create(
      ~body=Piaf.Body.of_string(message),
      `Internal_server_error,
    )
  | `Auth(message) =>
    Piaf.Response.create(~body=Piaf.Body.of_string(message), `Unauthorized)
  | _ =>
    Piaf.Response.create(
      ~body=Piaf.Body.of_string("Unkown server error"),
      `Internal_server_error,
    );

let response_of_result =
  fun
  | Ok(response) => response
  | Error(failure) => response_of_failure(failure);

let result_map = (fn, res) => {
  switch (res) {
  | Ok(data) => Ok(fn(data))
  | e => e
  };
};

let empty = Ok(Piaf.Response.create(`OK));

let make = (~version=?, ~headers=?, ~body=?, status) =>
  Piaf.Response.create(~version?, ~headers?, ~body?, status) |> Result.ok;

let add_header = ((name, value): (string, string)) =>
  result_map((res: Piaf.Response.t) => {
    let headers = Piaf.Headers.add(res.headers, name, value);
    Piaf.Response.create(~headers, ~body=res.body, res.status);
  });

let add_headers = (new_headers: list((string, string))) =>
  result_map((res: Piaf.Response.t) => {
    let headers = Piaf.Headers.add_list(res.headers, new_headers);
    Piaf.Response.create(~headers, ~body=res.body, res.status);
  });

let set_status = (status: Piaf.Status.t) =>
  result_map((res: Piaf.Response.t) =>
    Piaf.Response.create(~headers=res.headers, ~body=res.body, status)
  );

let set_body = body =>
  result_map((res: Piaf.Response.t) =>
    Piaf.Response.create(~headers=res.headers, ~body, res.status)
  );

let ok = () =>
  Ok(Piaf.Response.create(~body=Piaf.Body.of_string("ok"), `OK));
let text = text =>
  Ok(
    Piaf.Response.create(
      ~headers=
        Piaf.Headers.(add(empty, "Content-Type", "text/plain; charset=utf-8")),
      ~body=Piaf.Body.of_string(text),
      `OK,
    ),
  );

let json = json =>
  Ok(
    Piaf.Response.create(
      ~headers=Piaf.Headers.(add(empty, "Content-Type", "application/json")),
      ~body=Piaf.Body.of_string(json),
      `OK,
    ),
  );

let html = markup =>
  Ok(
    Piaf.Response.create(
      ~headers=
        Piaf.Headers.(add(empty, "Content-Type", "text/html; charset=utf-8")),
      ~body=Piaf.Body.of_string(markup),
      `OK,
    ),
  );

let redirect = (~code=303, targetPath) =>
  Ok(
    Piaf.Response.create(
      ~headers=Piaf.Headers.(add(empty, "Location", targetPath)),
      ~body=Piaf.Body.of_string(targetPath),
      `Code(code),
    ),
  );

let unauthorized = message =>
  Ok(
    Piaf.Response.create(~body=Piaf.Body.of_string(message), `Unauthorized),
  );

let not_found = (~message="Not found", ()) =>
  Ok(Piaf.Response.create(~body=Piaf.Body.of_string(message), `Not_found));

let string_stream = (~stream) => {
  Ok(Piaf.Response.of_string_stream(~body=stream, `OK));
};

let static = (~file_path) => {
  Lwt.catch(
    () => Piaf.Response.of_file(file_path) |> Lwt_result.ok,
    _ => Lwt_result.fail(`Server("File not found")),
  );
};
