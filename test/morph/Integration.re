open TestFramework;

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() =>
            Morph.start_server(~http_port=20000, _ =>
              Lwt.return(
                Morph_core.Response.empty
                |> Morph_core.Response.set_body(`String("test")),
              )
            )
          )
       |> afterAll(Lwt.cancel)
     )
  |> build;

describe("test lifecycle order example", ({test, describe}) => {
  test("some test", ({expect}) => {
    Morph_client.handle(Morph_core.Request.make("http://localhost:20000"))
    |> Lwt.map(response => {
         Morph_core.Response.(
           switch (response.body) {
           | `String(body) => expect.string(body).toEqual("test")
           | _ => expect.string("fail").toEqual("me")
           }
         )
       })
    |> Lwt_main.run;

    expect.assertions(1);
  })
});
