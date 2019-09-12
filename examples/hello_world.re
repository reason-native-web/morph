let handler = _request =>
  Morph_core.Response.text("Hello world!", Morph_core.Response.empty);

Morph.start_server(handler) |> Lwt_main.run;
