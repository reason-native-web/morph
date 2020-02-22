let response_to_valid = (response: Morph.Response.t) => {
  switch (response) {
  | Error(_) => response
  | Ok(success) =>
    switch (success.body) {
    | Morph.Response.String(_)
    | Morph_base.Response.Stream(_)
    | Morph_base.Response.StringStream(_) => response
    | _ => Error(`Server("Unsupported body type"))
    }
  };
};
