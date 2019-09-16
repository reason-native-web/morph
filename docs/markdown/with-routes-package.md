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
let root_handler = (_request) => Morph_core.Response.ok(Morph_core.Response.empty);

/* Return a greeting with the name */
let greet_handler = (greeting, _request) => {
  Morph_core.Response.text(greeting, Morph_core.Response.empty);
};
```

<!--OCaml-->

```ocaml
(* Just respond with "ok" on every request *)
let root_handler _request = Morph_core.Response.ok Morph_core.Response.empty

(* Return a greeting with the name *)
let greet_handler greeting _request = Morph_core.Response.text greeting Morph_core.Response.empty
```

<!--END_DOCUSAURUS_CODE_TABS-->

Then you create a route definition. This definition will use the `root_handler` for a empty route. If the request is `/greet/:name` it will match the last part and pass it as a string to the `greet_handler`.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let routes =
  Routes.(
    Routes.Infix.(
      with_method([
        (`GET, root_handler <$ s("")),
        (`GET, greet_handler <$> s("greet") *> str),
      ])
    )
  );
```

<!--OCaml-->

```ocaml
let routes =
  let open Routes in
    let open Routes.Infix in
      with_method [
        (`GET, root_handler <$ s "");
        (`GET, greet_handler <$> s "greet" *> str)
      ]
```

<!--END_DOCUSAURUS_CODE_TABS-->

Lastly you create a routes callback and start the server. In this case we pass in the request even if they are not used.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let handler = (request: Morph_core.Request.t) =>
  Routes.match_with_method(~target=request.target, ~meth=request.meth, routes)
  |> (
    fun
    | Some(res) => res(request)
    | None => Morph_core.Response.not_found(Morph_core.Response.empty)
  );

Morph.start_server(handler) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let () =
  let handler (request: Morph_core.Request.t) =
    (Routes.match_with_method ~target:request.target ~meth:request.meth routes)
    |> (function
      | Some res -> res request
      | None  -> Morph_core.Response.not_found Morph_core.Response.empty) in
  Morph.start_server handler
  |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
