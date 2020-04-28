type handler = Piaf.Server.Service.t(Request.t, Response.t);

type middleware = handler => handler;

type t = {
  start: handler => Lwt.t(unit),
  port: int,
};

let apply_all = (middlewares, handler) =>
  ListLabels.fold_left(~f=(|>), ~init=handler, middlewares);

let client_address_key: Hmap.key(Unix.sockaddr) = Hmap.Key.create();

let wrap_context = (next: handler): Piaf.Server.t(Unix.sockaddr) => {
  Lwt.Infix.(
    req => {
      let ctx = Hmap.add(client_address_key, req.ctx, Hmap.empty);
      next({...req, ctx}) >|= Response.response_of_result;
    }
  );
};

let make = (~port=8080, ~address=Unix.inet_addr_any, ()) => {
  let listen_address = Unix.(ADDR_INET(address, port));

  let start = handler => {
    Lwt.async(() => {
      open Lwt.Infix;
      let handler = sockaddr => sockaddr |> wrap_context(handler);

      let connection_handler = Piaf.Server.create(handler);

      Lwt_io.establish_server_with_client_socket(
        listen_address,
        connection_handler,
      )
      >|= (
        _server => Logs.info(m => m("HTTP server started on port: %d", port))
      );
    });

    let (forever, _) = Lwt.wait();
    forever;
  };

  {start, port};
};
