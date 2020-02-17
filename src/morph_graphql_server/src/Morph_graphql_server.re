let make =
    (schema: Graphql_lwt.Schema.schema('ctx))
    : Morph.Server.handler([> | `String(string)], 'res_body) => {
  open Morph.Request;
  let parse = ({context, body, _}: Morph.Request.t([> | `String(string)])) => {
    module Json = Yojson.Basic;
    let body =
      switch (body) {
      | `String(str) => str
      | _ => "{}"
      };
    let json = Json.from_string(body);
    let query = Json.Util.member("query", json) |> Json.Util.to_string_option;
    switch (query) {
    | Some(query_string) =>
      switch (Graphql_parser.parse(query_string)) {
      | Ok(query) =>
        let json_variables =
          Json.Util.member("variables", json)
          |> Json.Util.to_option(Json.Util.to_assoc);

        let variables = (
          json_variables :>
            option(list((string, Graphql_parser.const_value)))
        );
        let operation_name =
          Json.Util.member("operationName", json)
          |> Json.Util.to_string_option;
        Graphql_lwt.Schema.execute(
          schema,
          context,
          ~variables?,
          ~operation_name?,
          query,
        );
      | Error(err) =>
        Logs.err(m => m("Graphql_lwt parse error: %s", err));
        let error_json: Yojson.Basic.t = `Assoc([("error", `String(err))]);
        Error(error_json) |> Lwt.return;
      }
    | None => Error(`Assoc([("error", `String("no query"))])) |> Lwt.return
    };
  };

  let handler: Morph.Server.handler('req_body, 'res_body) =
    request => {
      Lwt.Infix.(
        parse(request)
        >>= (
          result =>
            switch (result) {
            | Ok(`Response(json)) =>
              Morph.Response.empty
              |> Morph.Response.json(Yojson.Basic.to_string(json))
            | Error(json) =>
              Morph.Response.empty
              |> Morph.Response.set_status(`Internal_server_error)
              |> Morph.Response.json(Yojson.Basic.to_string(json))
            | _ =>
              Morph.Response.empty
              |> Morph.Response.set_status(`Internal_server_error)
              |> Lwt.return
            }
        )
      );
    };

  handler;
};
