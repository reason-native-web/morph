---
id: graphql-server
title: Simple GraphQL server
---

We provide a simple library that helps you create a graphql server easily. The only thing you have to provide is a (ocaml-graphql) schema.

First we start of with a user type and a simple storage for them, in this case it's a list in a ref.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
type user = {
  id: int,
  name: string,
  email: string,
};

let users = ref([{id: 1, name: "Ulrik Strid", email: "ulrik@strid.com"}]);
```

<!--OCaml-->

```ocaml
type user = {id: int; name: string; email: string}

let users = ref [{id= 1; name= "Ulrik Strid"; email= "ulrik@strid.com"}]
```

<!--END_DOCUSAURUS_CODE_TABS-->

Then we define how to get the fields for our schema.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
open Graphql_lwt;

let user =
  Schema.(
    obj("user", ~doc="A user in the system", ~fields=_ =>
      [
        field("id", ~typ=non_null(int), ~args=Arg.[], ~resolve=(info, p) =>
          p.id
        ),
        field(
          "name", ~typ=non_null(string), ~args=Arg.[], ~resolve=(info, p) =>
          p.name
        ),
        field(
          "email", ~typ=non_null(string), ~args=Arg.[], ~resolve=(info, p) =>
          p.email
        ),
      ]
    )
  );
```

<!--OCaml-->

```ocaml
open Graphql_lwt

let user =
  let open Schema in
  obj "user" ~doc:"A user in the system" ~fields:(fun _ ->
      [ field "id" ~typ:(non_null int)
          ~args:
            (let open Arg in
            [])
          ~resolve:(fun _info p -> p.id)
      ; field "name" ~typ:(non_null string)
          ~args:
            (let open Arg in
            [])
          ~resolve:(fun _info p -> p.name)
      ; field "email" ~typ:(non_null string)
          ~args:
            (let open Arg in
            [])
          ~resolve:(fun _info p -> p.email) ])
```

<!--END_DOCUSAURUS_CODE_TABS-->

When we have our user defined as a schema type we can then create our mutations and queries.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let schema: Schema.schema(Hmap.t) =
  Schema.(
    schema(
      ~mutations=[
        field(
          "addUser",
          ~typ=non_null(user),
          ~args=
            Arg.[
              arg("name", non_null(string)),
              arg("email", non_null(string)),
            ],
          ~resolve=(_info, (), name, email) => {
            let new_user = {id: List.length(users^) + 1, name, email};
            users := [new_user, ...users^];
            new_user;
          },
        ),
      ],
      [
        field(
          "users",
          ~typ=non_null(list(non_null(user))),
          ~args=Arg.[],
          ~resolve=(_info, ()) =>
          users^
        ),
        field(
          "userById",
          ~typ=user,
          ~args=Arg.[arg("id", non_null(int))],
          ~resolve=(_info, (), id) =>
          List.find_opt(u => u.id == id, users^)
        ),
      ],
    )
  );
```

<!--OCaml-->

```ocaml
let schema : Hmap.t Schema.schema =
  let open Schema in
  schema
    ~mutations:
      [ field "addUser" ~typ:(non_null user)
          ~args:
            Arg.
              [ arg "name" ~typ:(non_null string)
              ; arg "email" ~typ:(non_null string) ]
          ~resolve:(fun _info () name email ->
            let new_user = {id= List.length !users + 1; name; email} in
            users := new_user :: !users ;
            new_user) ]
    [ field "users"
        ~typ:(non_null (list (non_null user)))
        ~args:Arg.[]
        ~resolve:(fun _info () -> !users)
    ; field "userById" ~typ:user
        ~args:Arg.[arg "id" ~typ:(non_null int)]
        ~resolve:(fun _info () id -> List.find_opt (fun u -> u.id = id) !users)
    ]
```

<!--END_DOCUSAURUS_CODE_TABS-->

Then we create our handler.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let handler = Morph_graphql_server.make(schema);
```

<!--OCaml-->

```ocaml
let handler = Morph_graphql_server.make schema
```

<!--END_DOCUSAURUS_CODE_TABS-->

When we have our handler we can create a server and start serving requests. This setup doesn't care about the route.

<!--DOCUSAURUS_CODE_TABS-->
<!--Reason-->

```reason
let server = Morph_server_http.make();

Morph.start(~servers=[server], handler) |> Lwt_main.run;
```

<!--OCaml-->

```ocaml
let () =
  let server = Morph_server_http.make () in
  Morph.start ~servers:[server] handler |> Lwt_main.run
```

<!--END_DOCUSAURUS_CODE_TABS-->
