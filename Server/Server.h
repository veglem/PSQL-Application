//
// Created by vzhur on 05.05.2023.
//

#pragma once

#define WIN32_LEAN_AND_MEAN
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

#include "Session.h"
#include "Heandler.h"



namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server>{
public:

    Server(const std::string& ip, unsigned short port, std::shared_ptr<Heandler> callbacks);
    void run();
private:

    void fail(beast::error_code ec, char const* what);
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
    net::io_context* socket_stream_;
    tcp::endpoint* ep_;
    tcp::acceptor* acceptor_;
    std::shared_ptr<Heandler> callbacks_;
};

//void on_accept(const beast::error_code &ec, Server* server);


