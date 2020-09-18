(lang dune 1.11)

(using fmt 1.2)

(name morph)

(version 0.6.1)

(maintainers "Ulrik Strid")

(authors "Ulrik Strid")

(source
 (github reason-native-web/morph))

(homepage https://reason-native-web.github.io)

(documentation https://reason-native-web.github.io/morph)

(generate_opam_files true)

(package
 (name morph)
 (synopsis "Abstraction over Piaf")
 (depends
  (ocaml
   (>= 4.06.1))
  (dune
   (>= 1.11))
  reason
  (lwt
   (>= 4.0.0))
  logs
  fmt
  hmap
  piaf
  routes
  session
  session-cookie
  session-cookie-lwt
  cookie))

(package
 (name morph_websocket)
 (synopsis "Websocket support for morph")
 (depends
  (ocaml
   (>= 4.06.1))
  (dune
   (>= 1.11))
  (lwt
   (>= 4.0.0))
  logs
  fmt
  piaf
  websocketaf
  digestif))

(package
 (name morph_graphql_server)
 (synopsis "Helpers for working with graphql and morph")
 (depends
  (ocaml
   (>= 4.06.1))
  (dune
   (>= 1.11))
  reason
  lwt
  logs
  morph
  graphql
  graphql-lwt))
