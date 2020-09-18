open Piaf

module MySubscriptionManager = struct
  type t = (string, unit -> unit) Hashtbl.t

  let subscriptions t = t

  let add = Hashtbl.add

  let create = Hashtbl.create ?random:None

  let remove = Hashtbl.remove

  let iter = Hashtbl.iter

  let find_opt = Hashtbl.find_opt

  let mem = Hashtbl.mem

  let clear = Hashtbl.clear
end

(* let subscriptions = MySubscriptionManager.create 10 *)

let subscriptions =
  Subscriptions_transport_ws.SubscriptionsManager.make
    (module MySubscriptionManager)
    (Hashtbl.create 10)

let sha1 s = s |> Digestif.SHA1.digest_string |> Digestif.SHA1.to_raw_string

let execute_subscription ctx schema ~variables ?operation_name query callback =
  let open Lwt.Infix in
  match Graphql_parser.parse query with
  | Ok doc ->
      Lwt.async (fun () ->
          Graphql_lwt.Schema.execute schema ctx ~variables ?operation_name doc
          >|= callback)
  | Error msg -> callback (Error (`String msg))

let request_handler (schema : Hmap.t Graphql_lwt.Schema.schema) :
    Morph.Server.handler =
  let websocket_handler ctx wsd =
    let frame =
      Subscriptions_transport_ws.on_recv subscriptions
        ~subscribe:(execute_subscription ctx schema)
        {
          schedule =
            (fun (stream, _destroy) ~on_recv ~on_close ->
              Lwt.async
                Lwt.Infix.(fun () -> Lwt_stream.closed stream >|= on_close);
              Lwt_stream.iter (fun item -> on_recv item) stream
              |> Lwt.ignore_result);
          destroy = (fun (_, destroy) -> destroy ());
        }
        wsd
    in
    let eof () =
      Format.eprintf "EOF\n%!";
      Websocketaf.Wsd.close wsd
    in
    { Websocketaf.Server_connection.frame; eof }
  in
  let error_handler wsd (`Exn exn) =
    let message = Printexc.to_string exn in
    let payload = Bytes.of_string message in
    Websocketaf.Wsd.send_bytes wsd ~kind:`Text payload ~off:0
      ~len:(Bytes.length payload);
    Websocketaf.Wsd.close wsd
  in
  let upgrade_handler ctx upgrade =
    let ws_conn =
      Websocketaf.Server_connection.create_websocket ~error_handler
        (websocket_handler ctx)
    in
    upgrade (Gluten.make (module Websocketaf.Server_connection) ws_conn)
  in
  let request_handler (req : Hmap.t Piaf.Server.ctx) =
    let httpaf_headers =
      Httpaf.Headers.of_rev_list (Headers.to_rev_list req.request.headers)
    in
    match
      Websocketaf.Handshake.upgrade_headers ~sha1
        ~request_method:req.request.meth httpaf_headers
    with
    | Ok upgrade_headers ->
        Response.upgrade
          ~headers:(Headers.of_list upgrade_headers)
          (upgrade_handler req.ctx)
        |> Lwt_result.return
    | Error _err_str ->
        let h = Morph_graphql_server.make schema in
        h req
  in
  request_handler
