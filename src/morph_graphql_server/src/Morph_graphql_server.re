let make = (schema: Graphql_lwt.Schema.schema('ctx)) => {
  let parse = (ctx, body) => {
    module Json = Yojson.Basic;
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
    (request: Morph.Request.t) => {
      Lwt.Infix.(
        request.read_body()
        >>= parse(request.context)
        >>= (
          result =>
            switch (result) {
            | Ok(`Response(json)) =>
              let json_string = Yojson.Basic.to_string(json);
              Morph.Response.empty |> Morph.Response.json(json_string);
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
