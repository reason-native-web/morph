type t('body) = {
  target: string,
  meth: Method.t,
  headers: list((string, string)),
  body: 'body,
  context: Hmap.t,
};

let make:
  (
    ~meth: Method.t=?,
    ~headers: list((string, string))=?,
    ~body: 'body,
    ~context: Hmap.t=?,
    string
  ) =>
  t('body);
