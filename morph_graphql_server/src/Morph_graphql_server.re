let make = (schema: Graphql_lwt.Schema.schema('ctx)) => {
  let parse = (~json, ~ctx) => {
    module Json = Yojson.Basic;
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
          ctx,
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

  let handler: Morph.Server.handler =
    ({ctx, request}: Morph.Request.t) => {
      let json_body = Lwt_result.map(
        Yojson.Basic.from_string,
        Piaf.Body.to_string(request.body),
      );

      Lwt.bind(json_body, fun
      | Error(_) => Error(`Server("Unknown error")) |> Lwt.return
      | Ok(json) => parse(~json, ~ctx) |> Lwt.map(result =>
           (
             switch (result) {
             | Ok(`Response(json)) =>
               Morph.Response.json(Yojson.Basic.to_string(json))
             | Error(json) =>
               Morph.Response.json(Yojson.Basic.to_string(json))
               |> Morph.Response.set_status(`Internal_server_error)
             | _ => Error(`Server("Unknown error"))
             }
           )
         ));
    };

  handler;
};
