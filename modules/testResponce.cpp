//
// Created by vzhur on 06.05.2023.
//

#include "testResponce.h"

http::response<http::string_body> get_testResponse(beast::error_code err, http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "text/html");
    response.keep_alive(req.keep_alive());
    response.body() = "sample body response";
    return response;
}