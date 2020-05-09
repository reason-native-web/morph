module MemorySession = {
  module Backend = {
    include Session.Lift.IO(Lwt, Session.Memory);
    let create = () => Session.Memory.create();
  };
  include Session_cookie_lwt.Make(Backend);

  let add_session = (backend, ~session) => {
    set(backend, session);
  };

  let get_session = (backend, request: Piaf.Request.t, ~key) => {
    let cookie_header = Piaf.Headers.get(request.message.headers, "Cookie");
    let cookie =
      Option.bind(cookie_header, value => {
        Cookie.cookies_of_header(("Cookie", value))
        |> List.find_opt(((cookie_key, _)) => 
          cookie_key == key
        )
      });
    switch (cookie) {
    | Some((k, v)) =>
      let cookie_str = Printf.sprintf("%s=%s", k, v);

      of_header(backend, key, ("Cookie", cookie_str))
      |> Lwt.map(
           fun
           | Ok(Some(value)) => Ok(value)
           | _ => Error(Session.S.Not_set),
         );
    | None => Lwt_result.fail(Session.S.Not_set)
    };
  };
};

type s = {
  session: MemorySession.t,
  path: string
};

module Key = {
  let key: Hmap.key(Hashtbl.t(string, s)) = Hmap.Key.create();
};

let backend = MemorySession.Backend.create();

let get = (request: Request.t, ~key) => {
  MemorySession.get_session(backend, request.request, ~key)
  |> Lwt_result.map((session: MemorySession.t) => session.value);
};

let set = (~expiry=?, ~path="/", request: Request.t, ~value, ~key) => {
  let session_p = MemorySession.generate(~expiry?, backend, value);
  
  Lwt.bind(
    session_p,
    session => {
      Hashtbl.add(Hmap.get(Key.key, request.ctx), key, {session, path});
      MemorySession.add_session(backend, ~session);
    },
  );
};

let middleware =
    (next: Server.handler, request: Request.t): Lwt.t(Response.t) => {
  let keys = Hashtbl.create(16);
  let ctx = Hmap.add(Key.key, keys, request.ctx);
  next({...request, ctx})
  |> Lwt.map((response: Response.t) => {
       let headers =
         Hashtbl.fold(
           (key, {session, path}, ls) => {
             [MemorySession.to_cookie_hdrs(~path, key, session), ...ls]
           },
           keys,
           [],
         )
         |> List.flatten;
       Response.add_headers(headers, response);
     });
};
