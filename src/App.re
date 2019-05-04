let str = ReasonReact.string;
let arr = ReasonReact.array;

type volumeInfo = {
  title: string,
  publishedDate: string,
};

type book = {
  id: string,
  meta: volumeInfo,
};

type books = {
  items: array(book),
};

module Decode = {
  let volumeInfo = volumeInfo =>
    Json.Decode.{
      title: volumeInfo |> field("title", string),
      publishedDate: volumeInfo |> field("publishedDate", string),
    };
  let book = book =>
    Json.Decode.{
      id: book |> field("id", string),
      meta: book |> field("volumeInfo", volumeInfo),
    };
  let books = books =>
    Json.Decode.{
      items: books |> field("items", array(book)),
    };
};

let getBookImg = (~id) =>
  "http://books.google.com/books/content?id=" ++ id ++ "&printsec=frontcover&img=1&zoom=1&source=gbs_api";

[@react.component]
let make = () => {
  let (searchTerm, setSearchTerm) = React.useState(() => "");
  let (books, setBooks) = React.useState(() => [||]);

  let fetchBooks = () =>
    Js.Promise.(
      Axios.get("https://www.googleapis.com/books/v1/volumes?q=" ++ searchTerm)
      |> then_(response => response##data)
      |> then_(json =>
          json |> Decode.books
               |> decodedBooks => setBooks(_ => decodedBooks.items)
               |> resolve
        )
      |> catch(error => resolve(Js.log(error)))
    );

  let onSearchTermChange = (event) => {
    setSearchTerm(ReactEvent.Form.target(event)##value);
  };

  let onSubmitSearchForm = (event) => {
    fetchBooks() |> ignore;
    ReactEvent.Mouse.preventDefault(event);
  };

  let booksList = Array.map(
    (book) =>
      <li key=book.id>
        <div>
          <img
            alt=book.meta.title
            src=getBookImg(book.id)
          />
          <h3>{str(book.meta.title)}</h3>
          <p>{str(book.meta.publishedDate)}</p>
        </div>
      </li>,
      books
  );

  <section>
    <a href="https://github.com/danbowles/reasonreact-hooks" title="Hooks Repo" target="_blank">
      {str("Github Repo")}
    </a>
    <form action="?">
      <label>
        <h1>{ReasonReact.string("Search for Books")}</h1>
        <input
          placeholder="microservice, restful, etc"
          value=(searchTerm)
          onChange=(onSearchTermChange)
        />
        <button onClick=(onSubmitSearchForm)>
          {str("Search")}
        </button>
      </label>
    </form>
    <ul>(arr(booksList))</ul>
  </section>
};
