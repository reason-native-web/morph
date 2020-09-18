let get_static_path = (req: Request.t, path) => {
  let path_length = String.length(path);
  let target_length = String.length(req.request.target);
  if (target_length + 1 <= path_length) {
    None;
  } else {
    try({
      let substr = String.sub(req.request.target, 1, path_length);

      if (substr == path) {
        Some(
          String.sub(
            req.request.target,
            path_length + 1,
            target_length - (path_length + 1),
          ),
        );
      } else {
        None;
      };
    }) {
    | _ => None
    };
  };
};

let make = (~path: string, ~public_path: string): Server.middleware => {
  (next: Server.handler, req: Request.t) =>
    switch (get_static_path(req, path)) {
    | Some(file_path) => Response.static(~file_path=public_path ++ file_path)
    | None => next(req)
    };
};
