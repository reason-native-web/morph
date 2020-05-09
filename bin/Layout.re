open Tyxml;

let createElement = (~title, ~children, ()) =>
  <html lang="en" className="text-gray-900 antialiased leading-tight">
    <head>
      <link
        href="https://unpkg.com/tailwindcss@1.4.5/dist/tailwind.min.css"
        rel="stylesheet"
      />
      <title> {Html.txt(title)} </title>
    </head>
    <body className="min-h-screen bg-gray-100"> ...children </body>
  </html>;
