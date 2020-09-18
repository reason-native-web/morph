open Tyxml;
let graphiql_script =
  Tyxml.Html.script(
    ~a=[
      Tyxml.Xml.string_attrib("type", "module") |> Tyxml.Html.to_attrib,
      Tyxml.Html.a_src("/public/graphiql.js"),
    ],
    Tyxml.Html.cdata_script(""),
  );

let make = () => {
  let crossorigin = `Anonymous;
  <html>
    <head>
      <title> "GraphiQL" </title>
      <style>
        {j|body {
        height: 100%;
        margin: 0;
        width: 100%;
        overflow: hidden;
      }

      #graphiql {
        height: 100vh;
      }|j}
      </style>
      <script
        crossorigin
        src="https://unpkg.com/react@16/umd/react.development.js"
      />
      <script
        crossorigin
        src="https://unpkg.com/react-dom@16/umd/react-dom.development.js"
      />
      <script
        crossorigin
        src="https://cdnjs.cloudflare.com/ajax/libs/graphiql/1.0.3/graphiql.js"
      />
      <script
        crossorigin
        src="https://unpkg.com/subscriptions-transport-ws@0.8.3/browser/client.js"
      />
      <script
        crossorigin
        src="https://unpkg.com/graphiql-subscriptions-fetcher@0.0.2/browser/client.js"
      />
      <link
        rel="stylesheet"
        href="https://unpkg.com/graphiql/graphiql.min.css"
      />
    </head>
    <body> <div id="graphiql"> "Loading..." </div> graphiql_script </body>
  </html>;
};
