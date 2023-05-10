//
// Created by vzhur on 06.05.2023.
//

#ifndef SERVER_TEST_TESTRESPONCE_H
#define SERVER_TEST_TESTRESPONCE_H

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

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class testResponce {

};

http::response<http::string_body> get_testResponse(beast::error_code, http::request<http::string_body>&);


#endif //SERVER_TEST_TESTRESPONCE_H
