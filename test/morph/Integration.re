open TestFramework;

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() =>
            Morph.start_server(~http_port=20000, _ =>
              Lwt.return(Morph_core.Response.empty)
            )
          )
       |> afterAll(Lwt.cancel)
     )
  |> build;

describe("test lifecycle order example", ({test, describe}) => {
  test("some test", ({expect}) =>
    Morph_client.handle(Morph_core.Request.make("http://localhost:20000"))
    |> Lwt.map(response => {
         Morph_core.Response.(
           switch (response.body) {
           | `String(body) => expect.string(body).toEqual("")
           | _ => expect.string("fail").toEqual("me")
           }
         )
       })
    |> Lwt_main.run
  )
});
