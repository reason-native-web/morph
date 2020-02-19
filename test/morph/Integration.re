open TestFramework;
let test_string = "test";
let test_port = 20000;

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => {
            let http_server =
              Morph_server_http.make(
                ~port=test_port,
                ~address=Unix.inet_addr_loopback,
                (),
              );
            Morph.start(~servers=[http_server], _ =>
              Lwt.return(
                Morph.Response.empty
                |> Morph.Response.add_header(("connection", "close"))
                |> Morph.Response.set_body(
                     Morph.Response.String(test_string),
                   ),
              )
            );
          })
       |> afterAll(Lwt.cancel)
     )
  |> build;

describe("Integration", ({test, describe}) => {
  test("Request to server", ({expect}) => {
    open Lwt.Infix;

    Piaf.Client.Oneshot.request(
      ~meth=`GET,
      Uri.of_string("http://localhost:" ++ string_of_int(test_port)),
    )
    >>= (
      res =>
        switch (res) {
        | Ok(body) => Piaf.Response.body(body) |> Piaf.Body.to_string
        | Error(error) => Lwt.return(error)
        }
    )
    >|= (body => expect.string(body).toEqual(test_string))
    |> Lwt_main.run;

    expect.assertions(1);
  })
});
