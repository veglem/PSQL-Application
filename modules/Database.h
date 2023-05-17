//
// Created by vzhur on 11.05.2023.
//

#ifndef BOOSTBEASTSERVER_DATABASE_H
#define BOOSTBEASTSERVER_DATABASE_H

#include <mutex>
#include <string>
#include <memory>
#include <map>
#include "libpq-fe.h"


class Database {

private:
    static Database* pinstance_;
    static std::mutex mutex_;
    static std::map<std::string, std::pair<std::time_t, PGconn*>> connections_list_;

protected:
    Database()= default;
    ~Database() = default;


public:

    Database(Database &other) = delete;

    void operator=(const Database &) = delete;

    static Database* GetInstance(const std::string& user, const std::string& pass);

    static PGconn* getConn(const std::string& user);
};



#endif //BOOSTBEASTSERVER_DATABASE_H
