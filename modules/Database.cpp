//
// Created by vzhur on 11.05.2023.
//

#include "Database.h"

Database* Database::pinstance_{nullptr};
std::mutex Database::mutex_;
std::map<std::string, std::pair<std::time_t, PGconn*>> Database::connections_list_;

Database *Database::GetInstance(const std::string& user, const std::string& pass) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new Database();
    }
    for (const auto& i : connections_list_) {
        if(std::time(nullptr) - i.second.first > 30 && i.first != user){
            connections_list_.erase(i.first);
        }
    }
    if (connections_list_.find(user) == connections_list_.end()) {
        std::string params = "host=localhost port=5432 dbname=mydb user=";
        params += user + " password=" + pass;
        PGconn* newConn = PQconnectdb(params.c_str());
        connections_list_.insert({user, {std::time(nullptr), newConn}});
    }
    return pinstance_;
}

PGconn *Database::getConn(const std::string &user) {
    return connections_list_[user].second;
}
