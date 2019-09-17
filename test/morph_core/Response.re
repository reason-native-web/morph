open TestFramework;

describe("Morph_core.Response", ({test, _}) => {
  open Morph_core;
  test("set_status", ({expect}) =>
    expect.equal(
      (Response.empty |> Response.set_status(`Not_found)).body,
      Response.make(~status=`Not_found, `String("")).body,
    )
  );

  test("set_body", ({expect}) =>
    expect.equal(
      Response.empty |> Response.set_body(`String("Hello")),
      Response.make(`String("Hello")),
    )
  );
});
