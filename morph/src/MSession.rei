let middleware: Server.middleware;

let get: (Request.t, ~key: string) => Lwt_result.t(string, Session.S.error);

let set:
  (
    ~expiry: int64=?,
    ~path: string=?,
    Request.t,
    ~value: string,
    ~key: string
  ) =>
  Lwt.t(unit);
