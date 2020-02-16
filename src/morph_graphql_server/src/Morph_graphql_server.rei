let make:
  Graphql_lwt.Schema.schema(Hmap.t) =>
  Morph.Server.handler([> | `String(string)], 'res_body);
