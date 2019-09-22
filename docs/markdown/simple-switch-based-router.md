---
id: simple-switch-based-router
title: Simple switch-based router
---

We can create a simple router with a switch statement. First we take the requests `target` and get the paths from it. We then split it on `/` and remove empty strings to get the `path_parts`.

We can then match on the mathod and the `path_parts`. Since patternmatching in Reason and OCaml and is very powerful you can even get the values out of the matching.

The example code bellow shows some common usecases

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let handler = (request: Morph.Request.t) => {
  let path_parts =
    request.target
    |> Uri.of_string
    |> Uri.path
    |> String.split_on_char('/')
    |> List.filter(s => s != "");

  switch (request.meth, path_parts) {
  | (_, []) => Morph.Response.text("Hello world!", Morph.Response.empty)
  | (_, ["greet", name]) =>
    Morph.Response.text("Hello " ++ name ++ "!", Morph.Response.empty)
  | (`GET, ["static", ...file_path]) =>
    Morph.Response.static(file_path |> String.concat("/"), Morph.Response.empty)
  | (_, _) => Morph.Response.not_found, Morph.Response.empty)
  };
};
```

<!--OCaml-->

```ocaml
let handler (request : Morph.Request.t) =
  let path_parts =
      request.target
      |> Uri.of_string
      |> Uri.path
      |> String.split_on_char '/'
      |> List.filter (fun s  -> s <> "") in
  match ((request.meth), path_parts) with
  | (_,[]) ->
      Morph.Response.text "Hello world!" Morph.Response.empty
  | (_,"greet"::name::[]) ->
      Morph.Response.text ("Hello " ^ name ^ "!") Morph.Response.empty
  | (`GET, "static"::file_path) ->
      Morph.Response.static (file_path |> String.concat "/") Morph.Response.empty
  | (_,_) -> Morph.Response.not_found Morph.Response.empty
```

<!--END_DOCUSAURUS_CODE_TABS-->

When you have built out the routes you want inside of the handler you simply pass it as the last argument to `Morph.start` to start the server.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let server = Morph_server_http.make();
Morph.start(~servers=[server], handler)
|> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let server = Morph_server_http.make () in
Morph.start ~servers:[server] handler
|> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
