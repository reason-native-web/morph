Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Info));
Logs.set_reporter(Logs_fmt.reporter());

let logger:
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

let proxy:
  Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t) =
  (
    service,
    {target, meth, headers, read_body, context}: Morph_core.Request.t,
  ) => {
    let host = "www.bing.com";
    let target = "https://" ++ host ++ target;
    Logs.info(m => m("Proxying request to: %s", target));
    let headers =
      List.filter(
        ((name, _)) => {
          switch (name) {
          | "Upgrade-Insecure-Requests" => false
          | "Host" => false
          | _ => true
          }
        },
        headers,
      )
      |> List.append([
           ("Via", "HTTP/1.1 localhost:8080"),
           ("X-Forwarded-For", "_secret"),
         ]);

    service({target, meth, headers, read_body, context});
  };

let handler = Morph_client.handle;

Morph.start_server(~middlewares=[logger, proxy], handler) |> Lwt_main.run;
