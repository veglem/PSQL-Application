#include <iostream>
#include "Server/Server.h"
#include "modules/testResponce.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::shared_ptr<Heandler> h = std::make_shared<Heandler>();

    h->addHeandler("/api/v1/test", http::verb::get, &get_testResponse);
//    http::response<http::string_body> r = h.heandlers_[{"api/v1/test", http::verb::get}](0 , 0);
    std::make_shared<Server> ("127.0.0.1", 80, std::move(h))->run();

    return 0;
}
