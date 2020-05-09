let make =
    (
      ~get: list(Routes.route(Morph.Server.handler))=[],
      ~post: list(Routes.route(Morph.Server.handler))=[],
      req: Morph.Request.t,
    ) => {
  let get_router = Routes.one_of(get);
  let post_router = Routes.one_of(post);

  let route_match =
    switch (req.request.message.meth) {
    | `GET => Routes.match'(~target=req.request.message.target, get_router)
    | `POST => Routes.match'(~target=req.request.message.target, post_router)
    | _ => None
    };

  switch (route_match) {
  | Some(handler) => handler(req)
  | None => Morph.Response.not_found() |> Lwt.return
  };
};
