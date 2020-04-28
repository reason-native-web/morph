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
  piaf
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
