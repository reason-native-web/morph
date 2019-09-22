let read = (~content_length, ~get_request_body, ~schedule_read, reqd) => {
  let bodyBuffer = Buffer.create(content_length);

  let (bodyPromise, promiseResolver) = Lwt.wait();

  let request_body = get_request_body(reqd);
  let rec on_read = (request_data, ~off, ~len) => {
    let request_data_bytes = Bytes.create(len);
    Lwt_bytes.blit_to_bytes(request_data, off, request_data_bytes, 0, len);
    Buffer.add_bytes(bodyBuffer, request_data_bytes);
    schedule_read(request_body, ~on_eof, ~on_read);
  }
  and on_eof = () => {
    Lwt.wakeup_later(promiseResolver, Buffer.contents(bodyBuffer));
  };

  schedule_read(request_body, ~on_eof, ~on_read);

  bodyPromise;
};
