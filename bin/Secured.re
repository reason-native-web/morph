open Tyxml;

let markup =
  <Layout title="login">
    <div className="max-w-sm w-full lg:max-w-full lg:flex">
      <div
        className="h-48 lg:h-auto lg:w-48 flex-none bg-cover rounded-t lg:rounded-t-none lg:rounded-l text-center overflow-hidden"
        style="background-image: url('https://placekitten.com/400/200')"
        title="Cute cat"
      />
      <div
        className="border-r border-b border-l border-gray-400 lg:border-l-0 lg:border-t lg:border-gray-400 bg-white rounded-b lg:rounded-b-none lg:rounded-r p-4 flex flex-col justify-between leading-normal">
        <div className="mb-8">
          <p className="text-sm text-gray-600 flex items-center">
            <svg
              className="fill-current text-gray-500 w-3 h-3 mr-2"
              viewBox="0 0 20 20">
              <path
                d="M4 8V6a6 6 0 1 1 12 0v2h1a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H3a2 2 0 0 1-2-2v-8c0-1.1.9-2 2-2h1zm5 6.73V17h2v-2.27a2 2 0 1 0-2 0zM7 6v2h6V6a3 3 0 0 0-6 0z"
              />
            </svg>
            {Html.txt("Members only")}
          </p>
          <div className="text-gray-900 font-bold text-xl mb-2">
            {Html.txt("Can coffee make you a better developer?")}
          </div>
          <p className="text-gray-700 text-base">
            {Html.txt(
               "Lorem ipsum dolor sit amet, consectetur adipisicing elit. Voluptatibus quia, nulla! Maiores et perferendis eaque, exercitationem praesentium nihil.",
             )}
          </p>
        </div>
      </div>
      <p className="text-center text-gray-500 text-xs">
        {Html.txt("&copy;2020 Acme Corp. All rights reserved.")}
      </p>
    </div>
  </Layout>;

let handler = _request => {
  Render.respond_html(markup);
};
