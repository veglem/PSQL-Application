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
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <thread>
#include <vector>

//#include "Session.h"
#include "../modules/testResponce.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Heandler {
    friend class Session;
public:
    void addHeandler(boost::string_view path, http::verb method,
                     const std::function<http::response<http::string_body>(beast::error_code,
                                                                     http::request<http::string_body>&)>&);
private:
    std::map<std::pair<
            boost::string_view, http::verb>, std::function<http::response<http::string_body>(beast::error_code,
                                                                                             http::request<http::string_body>&)>>
            heandlers_;

};




