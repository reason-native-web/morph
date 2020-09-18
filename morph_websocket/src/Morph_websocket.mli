(** Warning: This API is unstable and can break at any time *)

val handler :
  (send:(string -> unit) ->
  reader:((string -> unit) -> unit) ->
  close:(unit -> unit) ->
  unit) ->
  Morph.Server.handler
(** Create a websocket handler *)

module Graphql = Graphql_websocket
