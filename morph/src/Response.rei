type body = Piaf.Body.t;

type failure = [ | `User(string) | `Server(string) | `Auth(string)];

/**
The core [Response.t] type
*/
type t = result(Piaf.Response.t, failure);

let response_of_failure: failure => Piaf.Response.t;

let response_of_result: t => Piaf.Response.t;

/**
[make status headers body] creates a response.
*/
let make:
  (
    ~version: Piaf.Versions.HTTP.t=?,
    ~headers: Piaf.Headers.t=?,
    ~body: Piaf.Body.t=?,
    Piaf.Status.t
  ) =>
  t;

/**
[empty t] an empty response, a starting place to compose an http response.
*/
let empty: t;

/**
[add_header header response] returns a copy of t of response with the header tuple added.
*/
let add_header: ((string, string), t) => t;

/**
[add_header headers response] returns a copy of t of response with the headers added.
*/
let add_headers: (Header.t, t) => t;

/**
[set_status status response] returns a copy of t with the given status.
*/
let set_status: (Piaf.Status.t, t) => t;

/**
[set_body body response] returns a copy of t with the given body.
*/
let set_body: (body, t) => t;

/**
[ok response] is a conventience function to return a 200 OK response.
*/
let ok: unit => t;

/**
[text text response] is a conventience function to return a text response.
*/
let text: string => t;

/**
[json json response] is a conventience function to return a JSON response.
*/
let json: string => t;

/**
[html markup response] is a conventience function to return a HTML response.
*/
let html: string => t;

/**
[redirect code target] is a conventience function to create a redirect response.
*/
let redirect: (~code: int=?, string) => t;

/**
[unauthorized message] is a conventience function to return a unauthorized response.
*/
let unauthorized: string => t;

/**
[not_found message] is a conventience function to return a 404 Not found response.
*/
let not_found: (~message: string=?, unit) => t;

/**
  [static stream] is a conventience function to return a stream of strings.
  */
let string_stream: (~stream: Lwt_stream.t(string)) => t;

/**
  [static file_path] is a conventience function to return a file.
  */
let static: (~file_path: string) => t;
