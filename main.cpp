#include <iostream>
#include "Server/Server.h"
#include "modules/testResponce.h"
#include "modules/testResponseSecond.h"
#include "modules/responceHeaders.h"

int main() {

    std::shared_ptr<Heandler> h = std::make_shared<Heandler>();
    h->addHeandler("/api/v1/login", http::verb::get, &get_loginResponse);
    h->addHeandler("/api/v1/register", http::verb::post, &post_registerResponse);
    h->addHeandler("/api/v1/company", http::verb::get, &get_company);
    h->addHeandler("/api/v1/company", http::verb::post, &post_add_company);
    h->addHeandler("/api/v1/project", http::verb::get, &get_project);
    h->addHeandler("/api/v1/project", http::verb::post, &post_add_project);
    h->addHeandler("/api/v1/developer", http::verb::get, &get_developer);
    h->addHeandler("/api/v1/developer", http::verb::post, &post_add_developer);
    h->addHeandler("/api/v1/dev_team", http::verb::post, &post_add_dev_team);
    h->addHeandler("/api/v1/position", http::verb::get, &get_position);
    h->addHeandler("/api/v1/company", http::verb::put, &put_change_company);
    h->addHeandler("/api/v1/project", http::verb::put, &put_change_project);
    h->addHeandler("/api/v1/company", http::verb::delete_, &del_company);
    h->addHeandler("/api/v1/project", http::verb::delete_, &del_project);

    h->addHeandler("/api/v1/developer", http::verb::put, &put_developer);
    h->addHeandler("/api/v1/developer", http::verb::delete_, &delete_developer);
    h->addHeandler("/api/v1/position", http::verb::put, &put_position);
    h->addHeandler("/api/v1/position", http::verb::delete_, &delete_position);
    h->addHeandler("/api/v1/position", http::verb::post, &add_position);
    h->addHeandler("/api/v1/dev_team", http::verb::put, &put_dev_team);
    h->addHeandler("/api/v1/dev_team", http::verb::delete_, &delete_dev_team);
    h->addHeandler("/api/v1/dev_team", http::verb::get, &get_dev_team);
    h->addHeandler("/api/v1/users", http::verb::get, &post_search_user);
    h->addHeandler("/api/v1/dev/company", http::verb::get, &get_project_for_dev);

    h->addHeandler("/api/v1/order", http::verb::get, &get_testResponse);
    h->addHeandler("/api/v1/developer", http::verb::get, &get_testResponseSecond);
    std::make_shared<Server> ("127.0.0.1", 80, std::move(h))->run();
    return 0;
}
