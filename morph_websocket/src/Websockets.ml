open Piaf

let sha1 s = s |> Digestif.SHA1.digest_string |> Digestif.SHA1.to_raw_string

let request_handler communicate =
  let websocket_handler _client_address wsd =
    let send payload =
      let payload = Bytes.of_string payload in
      Websocketaf.Wsd.send_bytes wsd ~kind:`Text payload ~off:0
        ~len:(Bytes.length payload)
    in
    let frame ~opcode ~is_fin:_ bs ~off ~len =
      match opcode with
      | `Binary -> Websocketaf.Wsd.schedule wsd bs ~kind:`Binary ~off ~len
      | `Continuation ->
          Websocketaf.Wsd.schedule wsd bs ~kind:`Continuation ~off ~len
      | `Text ->
          let payload = Bytes.create len in
          let () = Lwt_bytes.blit_to_bytes bs off payload 0 len in
          communicate ~send
            ~reader:(fun read -> read (Bytes.unsafe_to_string payload))
            ~close:(fun () -> Websocketaf.Wsd.close wsd)
      | `Connection_close -> Websocketaf.Wsd.close wsd
      | `Ping -> Websocketaf.Wsd.send_pong wsd
      | `Pong | `Other _ -> ()
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
  let upgrade_handler addr upgrade =
    let ws_conn =
      Websocketaf.Server_connection.create_websocket ~error_handler
        (websocket_handler addr)
    in
    upgrade (Gluten.make (module Websocketaf.Server_connection) ws_conn)
  in
  let request_handler ({ request; ctx } : Hmap.t Piaf.Server.ctx) =
    let httpaf_headers =
      Httpaf.Headers.of_rev_list (Headers.to_rev_list request.headers)
    in
    let response =
      match
        Websocketaf.Handshake.upgrade_headers ~sha1 ~request_method:request.meth
          httpaf_headers
      with
      | Ok upgrade_headers ->
          Response.upgrade
            ~headers:(Headers.of_list upgrade_headers)
            (upgrade_handler ctx)
      | Error err_str ->
          Response.of_string ~body:err_str
            ~headers:(Headers.of_list [ ("Connection", "close") ])
            `Bad_request
    in
    Lwt.return response
  in
  request_handler
