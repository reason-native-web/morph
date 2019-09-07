/**
[handle request] is a [Opium_core.Service.t] that takes a [Morph_core.Request.t] and returns a [Morph_core.Response.t] wrapped in a [Lwt.t].

{[
    let handle: Morph_core.Request.t => Lwt.t(Morph_core.Response.t)
]}

*/
let handle: Opium_core.Service.t(Morph_core.Request.t, Morph_core.Response.t);
