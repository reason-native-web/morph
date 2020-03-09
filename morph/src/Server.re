type handler('req_body) = Request.t('req_body) => Lwt.t(Response.t);

type middleware('req_body) = handler('req_body) => handler('req_body);

type t('req_body) = {
  start: handler('req_body) => Lwt.t(unit),
  port: int,
};

let apply_all = (middlewares, handler) =>
  ListLabels.fold_left(~f=(|>), ~init=handler, middlewares);
