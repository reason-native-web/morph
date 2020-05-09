open Tyxml;

let login_form_markup =
  <Layout title="login">
    <div className="w-full max-w-xs">
      <form
        className="bg-white shadow-md rounded px-8 pt-6 pb-8 mb-4"
        action="/login"
        method="post">
        <div className="mb-4">
          <label
            className="block text-gray-700 text-sm font-bold mb-2"
            for_="username">
            "Username"
          </label>
          <input
            className="shadow appearance-none border rounded w-full py-2 px-3 text-gray-700 leading-tight focus:outline-none focus:shadow-outline"
            id="username"
            type_="text"
            name="username"
            placeholder="Username"
          />
        </div>
        <div className="mb-6">
          <label
            className="block text-gray-700 text-sm font-bold mb-2"
            for_="password">
            "Password"
          </label>
          <input
            className="shadow appearance-none border border-red-500 rounded w-full py-2 px-3 text-gray-700 mb-3 leading-tight focus:outline-none focus:shadow-outline"
            id="password"
            type_="password"
            name="password"
            placeholder="******************"
          />
          <p className="text-red-500 text-xs italic">
            "Please choose a password."
          </p>
        </div>
        <div className="flex items-center justify-between">
          <button
            className="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded focus:outline-none focus:shadow-outline"
            type_="submit">
            "Sign In"
          </button>
          <a
            className="inline-block align-baseline font-bold text-sm text-blue-500 hover:text-blue-800"
            href="#">
            "Forgot Password?"
          </a>
        </div>
      </form>
      <p className="text-center text-gray-500 text-xs">
        {Html.txt("&copy;2020 Acme Corp. All rights reserved.")}
      </p>
    </div>
  </Layout>;

let post_handler = (~session_key, req: Morph.Request.t) => {
  let referer = Piaf.Headers.get(req.request.message.headers, "Referer");

  Lwt.bind(
    Piaf.Body.to_string(req.request.body),
    form_data => {
      let (username, password) =
        String.split_on_char('&', form_data)
        |> List.map(String.split_on_char('='))
        |> (
          fun
          | [["username", username], ["password", password]] => (
              Some(username),
              Some(password),
            )
          | [["password", password], ["username", username]] => (
              Some(username),
              Some(password),
            )
          | [["username", username], [_]] => (Some(username), None)
          | [[_], ["password", password]] => (None, Some(password))
          | _ => (None, None)
        );

      switch (username, password) {
      | (Some("ulrik"), Some("test")) =>
        Morph.Session.set(
          req,
          ~path="/",
          ~key=session_key,
          ~value="logged in",
        )
        |> Lwt.map(() =>
             (
               switch (referer) {
               | Some(path) => path
               | None => "/secure"
               }
             )
             |> Morph.Response.redirect
           )
      | (Some(_), Some(_)) =>
        Morph.Response.redirect("/login?error=wrong_username_or_password")
        |> Lwt.return
      | (None, Some(_)) =>
        Morph.Response.redirect("/login?error=missing_username") |> Lwt.return
      | (Some(_), None) =>
        Morph.Response.redirect("/login?error=missing_password") |> Lwt.return
      | (None, None) =>
        Morph.Response.redirect("/login?error=missing_username_and_password")
        |> Lwt.return
      };
    },
  );
};

let post_routes = (~session_key) =>
  Routes.(one_of([s("login") /? nil @--> post_handler(~session_key)]));

let middleware =
    (~session_key="__session__", ~login_form_markup=login_form_markup)
    : Morph.Server.middleware =>
  (next, req) => {
    (
      switch (req.request.message.meth) {
      | `POST =>
        Routes.match'(
          ~target=req.request.message.target,
          post_routes(~session_key),
        )
      | _ => None
      }
    )
    |> (
      fun
      | Some(h) => h(req)
      | None =>
        Lwt.Infix.(
          Morph.Session.get(req, ~key=session_key)
          >>= (
            fun
            | Ok("logged in") => next(req)
            | _ => Render.respond_html(login_form_markup)
          )
        )
    );
  };
