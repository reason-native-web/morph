type handler('req_body, 'res_body) =
  Request.t('req_body) => Lwt.t(Response.t('res_body));

type middleware('req_body, 'res_body) =
  handler('req_body, 'res_body) => handler('req_body, 'res_body);

type t('req_body, 'res_body) = {
  start: handler('req_body, 'res_body) => Lwt.t(unit),
  port: int,
};

let apply_all = (middlewares, handler) =>
  ListLabels.fold_left(~f=(|>), ~init=handler, middlewares);
