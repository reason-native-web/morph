---
id: writing-middlewares
title: Writing middlewares for Morph
---

The base building block of `Morph` is `Middlewares` and `Handlers`.

> They are called `Filters` and `Services` in the underlying library, `Opium_core`.

A handler takes a request and returns a response and a middleware transforms (or should I say... Morphs) a handler. We're going to write a logger that write the time it takes for a request in this example. The signature of a middleware is:

```reason
Opium_core.Filter.simple(Morph_core.Request.t, Morph_core.Response.t)
```

which can be exapnded to:

```reason
Opium_core.Handler.t(Morph_core.Request.t, Morph_core.Response.t) => Opium_core.Handler.t(Morph_core.Request.t, Morph_core.Response.t)
```

or further:

```reason
(Morph_core.Request.t => Lwt.t(Morph_core.Response.t)) => (Morph_core.Request.t => Lwt.t(Morph_core.Response.t))
```

That means that a middleware that does nothing will look like this:

```reason
let middleware = service => {
    request => service(request);
};
```

To measure the time a request takes we basically need to have a middleware that checks the time when a request is received, lets the request finish, checks the time again and then logs the difference.

We're going to use the `Mtime` library to take accurate measurments of time in this example, the specific function is `Mtime_clock.elapsed()`. We're also going to use the `Logs` library to print the log statement.

The following code takes a service and returns a new service. The returned service saves the time and then calls the received service. Then it waits for the service to finish (`>|=` is basically `.then` if you're used to JavaScript promises). When the request is done we check how much time it took and calls the log function with something that will look like this: `http: GET request to /hello finished in 0.00674ms` and then we return the response. The middleware obviously have to be first in the chain to work.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let logger = (service) => (request: Morph_core.Request.t) => {
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
```

<!--OCaml-->

```ocaml
let logger service (request: Morph_core.Request.t) =
  let open Lwt.Infix in
  let start_request = Mtime_clock.elapsed () in
  service request
  >|= fun response ->
  let end_request = Mtime_clock.elapsed () in
  Logs.info (fun m ->
      m
        ("http: %s request to %s finished in %fms")
        (Morph_core.Method.to_string request.meth)
        request.target
        (Mtime.Span.abs_diff start_request end_request |> Mtime.Span.to_ms)) ;
  response;
```

<!--END_DOCUSAURUS_CODE_TABS-->

To use this middleware we can take the same code as in the getting started example and pass it first in the list to the labeled argument `middlewares`. We also need to do some setup for the logging library.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
Fmt_tty.setup_std_outputs();
Logs.set_level(Some(Logs.Info));
Logs.set_reporter(Logs_fmt.reporter());

let handler = _request => Morph_core.Response.text("Hello World!");

Morph.start_server(~middlewares=[logger], handler) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
Fmt_tty.setup_std_outputs ();
Logs.set_level (Some Logs.Info);
Logs.set_reporter (Logs_fmt.reporter ());

let handler _request = Morph_core.Response.text "Hello World!" Morph_core.Response.empty in
Morph.start_server ~middlewares:[logger] handler |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
