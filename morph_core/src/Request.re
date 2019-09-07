type t = {
  target: string,
  meth: Method.t,
  headers: list((string, string)),
  read_body: unit => Lwt.t(string),
  context: Hmap.t,
};
