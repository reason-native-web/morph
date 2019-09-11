(lang dune 1.11)
(name morph)
(source (github reason-native-web/morph))
(homepage https://reason-native-web.github.io)
(documentation https://reason-native-web.github.io/morph)
(generate_opam_files true)

(using fmt 1.2 (enabled_for reason))


(package
 (name morph_core)
 (synopsis "Base types defining request and response for a morph app")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  reason
  lwt
  opium_core
  hmap
  magic-mime
  ))


(package
 (name morph)
 (synopsis "Abstraction on Httpaf and H2 to run opium_core services")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
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
  opium_core
  hmap
  morph_core
  ))

(package
 (name morph_client)
 (synopsis "Abstraction on Httpaf to make http requests")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  reason
  lwt
  logs
  httpaf
  httpaf-lwt
  httpaf-lwt-unix
  ssl
  lwt_ssl
  opium_core
  morph_core
  hmap
  ))
