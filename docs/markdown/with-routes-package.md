---
id: with-routes-package
title: Usage with Routes library
---

If you want a nice performant router you can use the [Routes](https://github.com/anuragsoni/routes) library from [anuragsoni](https://github.com/anuragsoni).

First step is to create handlers. A handler is just a function that returns a `Response.t`.

> Note: We recommend that you include request in all handler signatures even if they are not used. That will make it easier to handle things like databases in the future.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
/* Just respond with "ok" on every request */
let root_handler = (_request) => Morph.Response.ok();

/* Return a greeting with the name */
let greet_handler = (greeting, _request) => {
  Morph.Response.text(greeting);
};
```

<!--OCaml-->

```ocaml
(* Just respond with "ok" on every request *)
let root_handler _request = Morph.Response.ok ()

(* Return a greeting with the name *)
let greet_handler greeting _request = Morph.Response.text greeting
```

<!--END_DOCUSAURUS_CODE_TABS-->

Then you create a route definition. This definition will use the `root_handler` for a empty route. If the request is `/greet/:name` it will match the last part and pass it as a string to the `greet_handler`.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let routes =
  Routes.(
    with_method([
      root_handler / s(""),
      greet_handler / s("greet") / str,
    ])
  );
```

<!--OCaml-->

```ocaml
let routes =
  let open Routes in
    one_of [
      s "" /? trail @--> root_handler;
      s "greet" / str /? trail @--> greet_handler;
    ]
```

<!--END_DOCUSAURUS_CODE_TABS-->

Lastly you create a routes callback and start the server. In this case we pass in the request even if they are not used.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let handler = {request, _} =>
  Routes.match(~target=request.message.target, routes)
  |> (
    fun
    | Some(res) => res(request)
    | None => Morph.Response.not_found(Morph.Response.empty)
  );

let server = Morph.Server.make();
Morph.start(~servers=[server], handler) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let () =
  let handler (req: Morph.Request.t) =
    (Routes.match' ~target:req.request.message.target routes)
    |> (function
      | Some handler -> handler req
      | None -> Morph.Response.not_found ())
    |> Lwt.return in
  let server = Morph.Server.make () in
  Morph.start ~servers:[server] handler
  |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
