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
let handler = ({request, _}: Morph.Request.t) => {
  let path_parts =
    request.message.target
    |> Uri.of_string
    |> Uri.path
    |> String.split_on_char('/')
    |> List.filter(s => s != "");

  switch (request.message.meth, path_parts) {
  | (_, []) => Morph.Response.text("Hello world!")
  | (_, ["greet", name]) =>
    Morph.Response.text("Hello " ++ name ++ "!")
  | (`GET, ["static", ...file_path]) =>
    Morph.Response.static(~file_path=String.concat("/", file_path))
  | (_, _) => Morph.Response.not_found()
  }
  |> Lwt.return;
};
```

<!--OCaml-->

```ocaml
let handler ({request; _}: Morph.Request.t) =
  let path_parts =
      request.message.target
      |> Uri.of_string
      |> Uri.path
      |> String.split_on_char '/'
      |> List.filter (fun s  -> s <> "") in
  (match (request.message.meth, path_parts) with
  | (_,[]) ->
      Morph.Response.text "Hello world!"
  | (_,"greet"::name::[]) ->
      Morph.Response.text ("Hello " ^ name ^ "!")
  | (`GET, "static"::file_path) ->
      Morph.Response.static ~file_path:(String.concat "/" file_path)
  | (_,_) -> Morph.Response.not_found ())
  |> Lwt.return
```

<!--END_DOCUSAURUS_CODE_TABS-->

When you have built out the routes you want inside of the handler you simply pass it as the last argument to `Morph.start` to start the server.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let server = Morph.Server.make();
Morph.start(~servers=[server], handler)
|> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let server = Morph.Server.make () in
Morph.start ~servers:[server] handler
|> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
