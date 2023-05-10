//
// Created by vzhur on 06.05.2023.
//

#ifndef SERVER_TEST_SESSION_H
#define SERVER_TEST_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <boost/bind/bind.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "Heandler.h"
#include "../modules/testResponce.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket str, std::shared_ptr<Heandler> callbacks);
    void run();
private:
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void fail(beast::error_code ec, char const* what);
    void do_close();
    void on_write(
            bool close,
            beast::error_code ec,
            std::size_t bytes_transferred);
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    std::shared_ptr<Heandler> callbacks_;
};


#endif //SERVER_TEST_SESSION_H
