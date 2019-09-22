open TestFramework;

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => {
            let http_server = Morph_server_http.make(~port=20000, ());
            Morph.start(~servers=[http_server], _ =>
              Lwt.return(
                Morph.Response.empty
                |> Morph.Response.set_body(`String("test")),
              )
            );
          })
       |> afterAll(Lwt.cancel)
     )
  |> build;

describe("test lifecycle order example", ({test, describe}) => {
  test("some test", ({expect}) => {
    Morph_client.handle(Morph.Request.make("http://localhost:20000"))
    |> Lwt.map(response => {
         Morph.Response.(
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
