/**
[headers] is represented as a list of (string, string) tuples.
*/
type headers = list((string, string));

/**
 [Response.body] variant type structure.  Either a flat [string], or an [Lwt_stream.t(char)].
 */
type body = [ | `String(string) | `Stream(Lwt_stream.t(char))];

/**
The core [Response.t] type
*/
type t = {
  status: Status.t,
  headers,
  body,
};

/**
[make status headers body] creates a response.
*/
let make: (~status: Status.t=?, ~headers: headers=?, body) => t;

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
let add_headers: (headers, t) => t;

/**
[set_status status response] returns a copy of t with the given status.
*/
let set_status: (Status.t, t) => t;

/**
[set_body body response] returns a copy of t with the given body.
*/
let set_body: (body, t) => t;

/**
[ok response] is a conventience function to return a 200 OK response.
*/
let ok: t => Lwt.t(t);

/**
[text text response] is a conventience function to return a text response.
*/
let text: (string, t) => Lwt.t(t);

/**
[json json response] is a conventience function to return a JSON response.
*/
let json: (string, t) => Lwt.t(t);

/**
[html markup response] is a conventience function to return a HTML response.
*/
let html: (string, t) => Lwt.t(t);

/**
[redirect code target response] is a conventience function to create a redirect response.
*/
let redirect: (~code: int=?, string, t) => Lwt.t(t);

/**
[unauthorized message response] is a conventience function to return a unauthorized response.
*/
let unauthorized: (string, t) => Lwt.t(t);

/**
[not_found message response] is a conventience function to return a 404 Not found response.
*/
let not_found: (~message: string=?, t) => Lwt.t(t);

/**
 [static file_path response] is a convenience function to return a 200 response with the contents of a static file.  If the file does not exist a 404 Not found response is sent instead.
 */
let static: (string, t) => Lwt.t(t);
