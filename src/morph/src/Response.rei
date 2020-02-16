/**
[headers] is represented as a list of (string, string) tuples.
*/
type headers = list((string, string));

/**
 [Response.body] variant type structure. There are currently 3 types of bodies.

 [`String] Use a simple string as body

 [`Stream] Use a stream of chars as body, the stream will be written fully before flushing

 [`StringStream] Use a stream of strings as body, it will be flushed on every new string

type body = [>
  | /** Use a simple string as body  */
    `String(string)
];
*/

type body('res_body) = [> | `String(string)] as 'res_body;

type success('res_body) = {
  status: Status.t,
  headers,
  body: body('res_body),
};

type failure = [ | `User(string) | `Server(string) | `Auth(string)];

/**
The core [Response.t] type
*/
type t('res_body) = result(success('res_body), failure);

let success_of_failure: failure => success('res_body);

/**
[make status headers body] creates a response.
*/
let make: (~status: Status.t=?, ~headers: headers=?, 'body) => t('body);

/**
[empty t] an empty response, a starting place to compose an http response.
*/
let empty: t('body);

/**
[add_header header response] returns a copy of t of response with the header tuple added.
*/
let add_header: ((string, string), t('body)) => t('body);

/**
[add_header headers response] returns a copy of t of response with the headers added.
*/
let add_headers: (headers, t('body)) => t('body);

/**
[set_status status response] returns a copy of t with the given status.
*/
let set_status: (Status.t, t('body)) => t('body);

/**
[set_body body response] returns a copy of t with the given body.
*/
let set_body: ('body, t('body)) => t('body);

/**
[ok response] is a conventience function to return a 200 OK response.
*/
let ok: t('body) => t('body);

/**
[text text response] is a conventience function to return a text response.
*/
let text: (string, t('body)) => t('body);

/**
[json json response] is a conventience function to return a JSON response.
*/
let json: (string, t('body)) => t('body);

/**
[html markup response] is a conventience function to return a HTML response.
*/
let html: (string, t('body)) => t('body);

/**
[redirect code target response] is a conventience function to create a redirect response.
*/
let redirect: (~code: int=?, string, t('body)) => t('body);

/**
[unauthorized message response] is a conventience function to return a unauthorized response.
*/
let unauthorized: (string, t('body)) => t('body);

/**
[not_found message response] is a conventience function to return a 404 Not found response.
*/
let not_found: (~message: string=?, t('body)) => t('body);

/**
 [static file_path response] is a co
nvenience function to return a 200 response with the contents of a static file.  If the file does not exist a 404 Not found response is sent instead.

let static: (string, t) => t;
 */;
