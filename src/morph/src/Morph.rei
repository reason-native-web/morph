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
    ~servers: list(Server.t('req_body, 'res_body)),
    ~middlewares: list(Server.middleware('req_body, 'res_body))=?,
    Server.handler('req_body, 'res_body)
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

module Method = Method;
module Status = Status;
