//
// Created by vzhur on 05.05.2023.
//

#include "Server.h"

Server::Server(const std::string &ip, unsigned short port, std::shared_ptr<Heandler> callbacks) : callbacks_(std::move(callbacks)){
    socket_stream_ = new boost::asio::io_context;

    ep_ = new boost::asio::ip::tcp::endpoint(net::ip::make_address(ip), port);
    acceptor_ = new boost::asio::ip::tcp::acceptor(*socket_stream_);



    beast::error_code error;
    acceptor_->open(ep_->protocol(), error);
    if (error) {
        fail(error, "open");

    }

    acceptor_->set_option(net::socket_base::reuse_address(true), error);
    if (error) {
            fail(error, "set_option");
        return;
    }



    // Bind to the server address
    acceptor_->bind(*ep_, error);
    if (error) {
            fail(error, "bind");

    }

    // Start listening for connections
    acceptor_->listen(
            net::socket_base::max_listen_connections, error);
    if (error) {
            fail(error, "listen");
        return;
    }
}



void Server::run() {

    do_accept();
    socket_stream_->run();
}

void Server::fail(beast::error_code ec, const char *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void Server::do_accept() {
    acceptor_->async_accept(net::make_strand(*socket_stream_), beast::bind_front_handler(&Server::on_accept, shared_from_this()));
}


void Server::on_accept(beast::error_code ec, tcp::socket socket) {
    std::make_shared<Session>(std::move(socket), callbacks_)->run();
    do_accept();
}


