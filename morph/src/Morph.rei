/**
[start servers middlewares handler] starts the server.

{2 Simple usage:}
{[
Morph.start(_request => Http.Response.Ok.make)
|> Lwt_main.run;
]}
*/
let start:
  (
    ~servers: list(Server.t),
    ~middlewares: list(Server.middleware)=?,
    Server.handler
  ) =>
  Lwt.t(unit);

module Server = Server;

/**
Defines the [request] type
*/
module Request = Request;

/**
Defines the [response] type and helper functions to easily create responses in [Morph].
*/
module Response = Response;

module Session = MSession;
