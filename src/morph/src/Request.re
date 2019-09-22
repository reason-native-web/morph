type t = {
  target: string,
  meth: Method.t,
  headers: list((string, string)),
  read_body: unit => Lwt.t(string),
  context: Hmap.t,
};

let make =
    (
      ~meth=`GET,
      ~headers=[],
      ~read_body=() => Lwt.return(""),
      ~context=Hmap.empty,
      target,
    ) => {
  target,
  meth,
  headers,
  read_body,
  context,
};
