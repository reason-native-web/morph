---
id: http-getting-started
title: Getting started
---

## Hello World!

To get started with your first web server simply call the `start_server` function with a handler function. The handler functions has the signature `Request.t => Response.t`. Where a `Request.t` is a record with everything you need from the underlying request and a `Response.t` is everything we need to create a response, `status`, `headers` and a `body`.

There are some helpers to create some of the common responses, we will use the text response for a "Hello World!" response.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let handler = _request => Morph_core.Response.text("Hello World!");

Morph.start_server(handler) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let () =
    let handler _request =
        Morph_core.Response.text "Hello World!" Morph_core.Response.empty in
    Morph.start_server handler |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->

The code above will answer every request with the text "Hello World!".

Congratulations you have now created your first Morph server running natively.

## Pesy template

There is also a [`pesy`](https://github.com/esy/pesy) template to help you get started quickly. If you don't have [`esy`](https://esy.sh) and `pesy` installed you can get them from `npm` with the following command:

```
npm install -g esy pesy@next
```

When you have installed them you can create a folder, cd into it and run the pesy command pointing to our template.

```
mkdir my_project
cd my_project
pesy --template=reason-native-web/morph-hello-world-pesy-template
```

You should now have a working project that you can play around with and even build on to get to production.
