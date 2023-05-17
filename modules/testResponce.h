//
// Created by vzhur on 06.05.2023.
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
#include <boost/json.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include <winsock2.h>

#include "libpq-fe.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class testResponce {

};

http::response<http::string_body> get_testResponse(beast::error_code, http::request<http::string_body>&);



