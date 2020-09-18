let handler com request =
  Websockets.request_handler com request |> Lwt_result.ok

module Graphql = Graphql_websocket
