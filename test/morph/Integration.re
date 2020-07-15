open TestFramework;
let test_string = "test";
let test_port = 20000;

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => {
            let http_server =
              Morph.Server.make(
                ~port=test_port,
                ~address=Unix.inet_addr_loopback,
                (),
              );
            Morph.start(~servers=[http_server], _ =>
              Lwt_result.return(
                Piaf.Response.create(
                  ~body=Piaf.Body.of_string(test_string),
                  ~headers=
                    Piaf.Headers.add(
                      Piaf.Headers.empty,
                      "content-length",
                      String.length(test_string) |> string_of_int,
                    ),
                  `OK,
                ),
              )
            );
          })
       |> afterAll(Lwt.cancel)
     )
  |> build;

describe("Integration", ({test, describe}) => {
  test("Request to server", ({expect}) => {
    open Lwt_result.Infix;

    Piaf.Client.Oneshot.request(
      ~meth=`GET,
      Uri.of_string("http://localhost:" ++ string_of_int(test_port)),
    )
    >>= (({body, _}: Piaf.Response.t) => Piaf.Body.to_string(body))
    |> Lwt.map(
         fun
         | Ok(body) => expect.string(body).toEqual(test_string)
         | Error(e) =>
           //  if we hit this it will fail to match and tell us the error
           expect.string(Piaf.Error.to_string(e)).toEqual(test_string),
       )
    |> Lwt_main.run;

    expect.assertions(1);
  })
});
