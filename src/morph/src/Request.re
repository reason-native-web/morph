type t('body) = {
  target: string,
  meth: Method.t,
  headers: list((string, string)),
  body: 'body,
  context: Hmap.t,
};

let make = (~meth=`GET, ~headers=[], ~body, ~context=Hmap.empty, target) => {
  target,
  meth,
  headers,
  body,
  context,
};
