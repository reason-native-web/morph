open Graphql_lwt;

let set_interval = (s, f, destroy) => {
  let rec set_interval_loop = (s, f, n) => {
    let timeout =
      Lwt_timeout.create(s, () =>
        if (n > 0) {
          f();
          set_interval_loop(s, f, n - 1);
        } else {
          destroy();
        }
      );

    Lwt_timeout.start(timeout);
  };

  set_interval_loop(s, f, 5);
};

type user = {
  id: int,
  name: string,
  email: string,
};

let users =
  ref([
    {id: 1, name: "Ulrik Strid", email: "ulrik@strid.com"},
    {id: 2, name: "Cem Turan", email: "cem2ran@gmail.com"},
  ]);

let user =
  Schema.(
    obj("user", ~doc="A user in the system", ~fields=_ =>
      [
        field("id", ~typ=non_null(int), ~args=Arg.[], ~resolve=(_info, user) =>
          user.id
        ),
        field(
          "name", ~typ=non_null(string), ~args=Arg.[], ~resolve=(_info, user) =>
          user.name
        ),
        field(
          "email",
          ~typ=non_null(string),
          ~args=Arg.[],
          ~resolve=(_info, user) =>
          user.email
        ),
      ]
    )
  );

let schema: Schema.schema(Hmap.t) =
  Schema.(
    schema(
      ~mutations=[
        field(
          "addUser",
          ~typ=non_null(user),
          ~args=
            Arg.[
              arg("name", ~typ=non_null(string)),
              arg("email", ~typ=non_null(string)),
            ],
          ~resolve=(_info, (), name, email) => {
            let new_user = {id: List.length(users^) + 1, name, email};
            users := [new_user, ...users^];
            new_user;
          },
        ),
      ],
      ~subscriptions=[
        subscription_field(
          "subscribe_to_user",
          ~typ=non_null(user),
          ~args=Arg.[arg'("intarg", ~typ=int, ~default=0)],
          ~resolve=(_info, _intarg) => {
            let (user_stream, push_to_user_stream) = Lwt_stream.create();
            let destroy_stream = () => push_to_user_stream(None);
            set_interval(
              2,
              () => {
                let idx = Random.int(List.length(users^));
                push_to_user_stream(Some(List.nth(users^, idx)));
              },
              destroy_stream,
            );
            Lwt_result.return((user_stream, destroy_stream));
          },
        ),
      ],
      [
        field(
          "users",
          ~typ=non_null(list(non_null(user))),
          ~args=Arg.[],
          ~resolve=(_info, ()) =>
          users^
        ),
        field(
          "userById",
          ~typ=user,
          ~args=Arg.[arg("id", ~typ=non_null(int))],
          ~resolve=(_info, (), id) =>
          List.find_opt(u => u.id == id, users^)
        ),
      ],
    )
  );
