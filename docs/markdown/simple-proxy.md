---
id: simple-proxy
title: A simple proxy using Morph_client
---

To create a proxy in Morph we can use the fact that `Morph_client` is just a handler (has the signature `Request.t => Lwt.t(Response.t)`). Because of that we can just create a middleware that changes the target to the remote we want to proxy and adds and removes headers as needed.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let proxy:
  Opium_core.Filter.simple(Morph.Request.t, Morph.Response.t) =
  (
    service,
    {target, meth, headers, read_body, context}: Morph.Request.t,
  ) => {
    let host = "www.bing.com";
    let target = "https://" ++ host ++ target;
    Logs.info(m => m("Proxying request to: %s", target));
    let headers =
      List.filter(
        ((name, _)) => {
          switch (name) {
          | "Host" => false /* TODO: This should be handled by the client */
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
```

<!--OCaml-->

```ocaml
let proxy service ({target; meth; headers; read_body; context} : Morph.Request.t) =
  let host = "www.bing.com" in
  let target = "https://" ^ host ^ target in
  let () = Logs.info (fun m -> m "Proxying request to: %s" target) in
  let headers =
    List.filter
      (fun (name, _) ->
        match name with
        | "Host" -> false (* TODO: This should be handled by the client  *)
        | _ -> true)
      headers
    |> List.append
         [("Via", "HTTP/1.1 localhost:8080"); ("X-Forwarded-For", "_secret")]
  in
  service Morph.Request.{target; meth; headers; read_body; context};
```

<!--END_DOCUSAURUS_CODE_TABS-->

Now that we hace our middleware we can pass that in the middlewares list and since the Morph_client is just a handler we can pass that as the handler argument.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let server = Morph_server_http.make();
Morph.start(~servers=[server], ~middlewares=[proxy], Morph_client.handle) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let () =
    let server = Morph_server_http.make () in
    Morph.start ~servers:[server] ~middlewares:[proxy] Morph_client.handle |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
