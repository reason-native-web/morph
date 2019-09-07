/**
[headers] is represented as a list of (string, string) tuples.
  */
type headers = list((string, string));

/**
 [Response.body] variant type structure.  Either a flat [string], or a [Buffer.t].
 */
type body =
  | String(string)
  | Stream(Lwt_stream.t(string));

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
[ok extra_headers unit] is a conventience function to return a 200 OK response.
*/
let ok: (~extra_headers: headers=?, unit) => Lwt.t(t);

/**
[text extra_headers text] is a conventience function to return a text response.
*/
let text: (~extra_headers: headers=?, string) => Lwt.t(t);

/**
[json extra_headers json] is a conventience function to return a JSON response.
*/
let json: (~extra_headers: headers=?, string) => Lwt.t(t);

/**
[html extra_headers markup] is a conventience function to return a HTML response.
*/
let html: (~extra_headers: headers=?, string) => Lwt.t(t);

/**
[redirect extra_headers code target] is a conventience function to create a redirect response.
*/
let redirect: (~extra_headers: headers=?, ~code: int=?, string) => Lwt.t(t);

/**
[unauthorized extra_headers message] is a conventience function to return a unauthorized response.
*/
let unauthorized: (~extra_headers: headers=?, string) => Lwt.t(t);

/**
[not_found extra_headers message unit] is a conventience function to return a 404 Not found response.
*/
let not_found:
  (~extra_headers: headers=?, ~message: string=?, unit) => Lwt.t(t);

/**
 [static extra_headers file_path] is a convenience function to return a 200 response with the contents of a static file.  If the file does not exist a 404 Not found response is sent instead.
 */
let static: (~extra_headers: headers=?, string) => Lwt.t(t);