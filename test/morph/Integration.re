open TestFramework;
let test_string = "test";

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => {
            let http_server = Morph_server_http.make(~port=20000, ());
            Morph.start(~servers=[http_server], _ =>
              Lwt.return(
                Morph.Response.empty
                |> Morph.Response.set_body(`String(test_string)),
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
      Uri.of_string("http://localhost:20000"),
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
