Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Info));
Logs.set_reporter(Logs_fmt.reporter());

let root_handler = _request =>
  Morph_core.Response.text("Hello world!", Morph_core.Response.empty());

let greet_handler = (greeting, _request) => {
  Morph_core.Response.text("Hello " ++ greeting, Morph_core.Response.empty());
};

let static_handler = _request => {
  let path_to_static_file = Sys.getcwd() ++ "/README.md";
  Morph_core.Response.empty()
  |> Morph_core.Response.static(path_to_static_file);
};

let routes =
  Routes.(
    Routes.Infix.(
      with_method([
        (`GET, root_handler <$ s("")),
        (`GET, greet_handler <$> s("greet") *> str),
        (`GET, static_handler <$ s("static")),
      ])
    )
  );

let middleware:
  Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t) =
  (service, request: Morph_core.Request.t) => {
    open Lwt.Infix;
    let start_request = Mtime_clock.elapsed();
    service(request)
    >|= (
      response => {
        let end_request = Mtime_clock.elapsed();
        Logs.info(m =>
          m(
            "http: %s request to %s finished in %fms",
            Morph_core.Method.to_string(request.meth),
            request.target,
            Mtime.Span.abs_diff(start_request, end_request)
            |> Mtime.Span.to_ms,
          )
        );

        response;
      }
    );
  };

let handler = (request: Morph_core.Request.t) => {
  Routes.match_with_method(~target=request.target, ~meth=request.meth, routes)
  |> (
    fun
    | Some(res) => res(request)
    | None => Morph_core.Response.not_found(Morph_core.Response.empty())
  );
};

Morph.start_server(~middlewares=[middleware], handler) |> Lwt_main.run;