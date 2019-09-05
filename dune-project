(lang dune 1.11)
(name morph)
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
  lwt
  opium_core
  hmap
  ))


(package
 (name morph)
 (synopsis "Abstraction on Httpaf and H2 to run opium_core services")
 (depends
  (ocaml
   (>= 4.04.2))
  (dune
   (>= 1.11))
  lwt
  logs
  httpaf
  httpaf-lwt-unix
  h2
  h2-lwt-unix
  opium_core
  hmap
  morph_core
  ))
