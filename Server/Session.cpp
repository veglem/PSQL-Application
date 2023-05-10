//
// Created by vzhur on 06.05.2023.
//

#include "Session.h"

void Session::run() {
    net::dispatch(stream_.get_executor(),
                  beast::bind_front_handler(
                          &Session::do_read,
                          shared_from_this()));
}

Session::Session(tcp::socket str, std::shared_ptr<Heandler> callbacks) : stream_(std::move(str)), callbacks_(std::move(callbacks)){

}

void Session::do_read() {
    req_ = {};

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(stream_, buffer_, req_,
                     beast::bind_front_handler(
                             &Session::on_read,
                             shared_from_this()));
}

void Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    if(ec == http::error::end_of_stream)
        return do_close();

    if(ec) {
        return fail(ec, "read");
    }

    std::string path = req_.target();
    path = path.substr(0, path.find('?'));
    if (callbacks_->heandlers_.find({path, req_.method()}) != callbacks_->heandlers_.end()){
//        res_ = callbacks_->heandlers_.find({path, req_.method()});
        res_ = (callbacks_->heandlers_[{path, http::verb::get}](ec, req_));

    } else{
        res_ = {http::status::not_found, req_.version()};
    }
//    res_ = get_testResponse(ec, req_);

    http::async_write(stream_, res_, beast::bind_front_handler(&Session::on_write, shared_from_this(), res_.need_eof()));
}

void Session::fail(beast::error_code ec, const char *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void Session::do_close() {
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}

void Session::on_write(bool close, beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    if(close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
    }

    // We're done with the response so delete it
    res_ = {};
    // Read another request
    do_read();
}






