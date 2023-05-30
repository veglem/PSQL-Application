//
// Created by vzhur on 15.05.2023.
//

#ifndef BOOSTBEASTSERVER_RESPONCEHEADERS_H
#define BOOSTBEASTSERVER_RESPONCEHEADERS_H

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
#include <ctime>

#include <winsock2.h>

#include "libpq-fe.h"
#include "Database.h"


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

http::response<http::string_body> get_loginResponse(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_registerResponse(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_company(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_add_company(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_project(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_add_project(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_developer(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_add_developer(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_position(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_add_dev_team(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> put_change_company(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> del_company(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> put_change_project(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> del_project(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> put_developer(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> delete_developer(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> put_position(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> delete_position(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> add_position(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> put_dev_team(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> delete_dev_team(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_dev_team(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> post_search_user(beast::error_code err, http::request<http::string_body>& req);

http::response<http::string_body> get_project_for_dev(beast::error_code err, http::request<http::string_body> &req);

#endif //BOOSTBEASTSERVER_RESPONCEHEADERS_H

