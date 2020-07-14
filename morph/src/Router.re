let color_of_method = method => {
  switch (method) {
  | "GET" => Pastel.Green
  | "POST" => Pastel.Magenta
  | "PUT" => Pastel.Blue
  | "DELETE" => Pastel.Red
  | _ => Pastel.Yellow
  };
};

let print_routes = (~routes, ~method, ()) => {
  let route_strings =
    List.map(r => {Format.asprintf("%a", Routes.pp_route, r)}, routes)
    |> List.sort((a, b) => String.length(a) - String.length(b))
    |> List.map(r => Pastel.make(~italic=true, [r]));

  let pretty_method =
    Pastel.make(~color=color_of_method(method), ~bold=true, [method, "\n"]);

  let routes_string = pretty_method ++ String.concat("\n", route_strings);

  print_endline(routes_string);
};

type route = Routes.route(Server.handler);

let make =
    (
      ~get: list(route)=[],
      ~post: list(route)=[],
      ~put: list(route)=[],
      ~del: list(route)=[],
      ~print=true,
      (),
    ) => {
  let get_router = Routes.one_of(get);
  let post_router = Routes.one_of(post);
  let put_router = Routes.one_of(put);
  let del_router = Routes.one_of(del);

  if (print) {
    print_endline(
      Pastel.make(~bold=true, ~underline=true, ["Routing table", "\n"]),
    );
    if (List.length(get) >= 1) {
      print_routes(~routes=get, ~method="GET", ());
      print_newline();
    };

    if (List.length(post) >= 1) {
      print_routes(~routes=get, ~method="POST", ());
      print_newline();
    };

    if (List.length(put) >= 1) {
      print_routes(~routes=put, ~method="PUT", ());
      print_newline();
    };

    if (List.length(del) >= 1) {
      print_routes(~routes=del, ~method="DELETE", ());
      print_newline();
    };
  };

  (req: Request.t) => {
    let route_match =
      switch (req.request.meth) {
      | `GET => Routes.match'(~target=req.request.target, get_router)
      | `POST => Routes.match'(~target=req.request.target, post_router)
      | `PUT => Routes.match'(~target=req.request.target, put_router)
      | `DELETE => Routes.match'(~target=req.request.target, del_router)
      | _ =>
        Logs.warn(m =>
          m(
            "Unsupported method %s",
            req.request.meth |> Httpaf.Method.to_string,
          )
        );
        None;
      };

    switch (route_match) {
    | Some(handler) => handler(req)
    | None => Response.not_found() |> Lwt.return
    };
  };
};
