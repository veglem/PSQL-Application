//
// Created by vzhur on 06.05.2023.
//

#include "Heandler.h"

void Heandler::addHeandler(boost::string_view path, http::verb method,
                           const std::function<http::response<http::string_body>(beast::error_code,
                                                                           http::request<http::string_body>&)>& function) {
    heandlers_.insert({{path, method}, function});
}