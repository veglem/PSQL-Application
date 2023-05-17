//
// Created by vzhur on 12.05.2023.
//

#ifndef BOOSTBEASTSERVER_TESTRESPONSESECOND_H
#define BOOSTBEASTSERVER_TESTRESPONSESECOND_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <boost/bind/bind.hpp>
#include "boost/json.hpp"

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
#include "Database.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


class testResponseSecond {

};

http::response<http::string_body> get_testResponseSecond(beast::error_code, http::request<http::string_body>&);

#endif //BOOSTBEASTSERVER_TESTRESPONSESECOND_H
