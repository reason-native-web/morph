/**
[handle request] is a [Opium_core.Service.t] that takes a [Morph.Request.t] and returns a [Morph.Response.t] wrapped in a [Lwt.t].

{[
    let handle: Morph.Request.t => Lwt.t(Morph.Response.t)
]}

*/
let handle: Opium_core.Service.t(Morph.Request.t, Morph.Response.t);
