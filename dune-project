(lang dune 1.11)
(name morph)
(source (github reason-native-web/morph))
(homepage https://reason-native-web.github.io)
(documentation https://reason-native-web.github.io/morph)
(generate_opam_files true)

(using fmt 1.2)

(package
 (name morph)
 (synopsis "Abstraction on Httpaf and H2")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  reason
  lwt
  logs
  hmap
  ))

  (package
 (name morph_base)
 (synopsis "Abstraction on Httpaf and H2")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  reason
  lwt
  logs
  ssl
  lwt_ssl  
  hmap
  magic-mime
  morph
  ))

(package
 (name morph_server_base)
 (synopsis "Abstraction on Httpaf and H2")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  morph
  reason
  lwt
  magic-mime
  ))

(package
 (name morph_server_http)
 (synopsis "Abstraction on Httpaf and H2")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  morph
  reason
  lwt
  logs
  httpaf
  httpaf-lwt
  httpaf-lwt-unix
  hmap
  ))

(package
 (name morph_server_https)
 (synopsis "Abstraction on Httpaf and H2")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  morph
  reason
  lwt
  logs
  httpaf
  httpaf-lwt
  httpaf-lwt-unix
  h2
  h2-lwt
  h2-lwt-unix
  ssl
  lwt_ssl
  hmap
  ))

(package
 (name morph_graphql_server)
 (synopsis "Helpers for working with graphql and morph")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  reason
  lwt
  logs
  morph
  graphql
  graphql-lwt
  ))

(package
  (name morph_test)
  (depends
    (ocaml (>= 4.04.2))
    (dune (>= 1.11))
    reason
    rely
    morph))
