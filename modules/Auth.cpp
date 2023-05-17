//
// Created by vzhur on 15.05.2023.
//

#include "responceHeaders.h"

http::response<http::string_body> get_loginResponse(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto sepPos = req.target().find("&user=");
    std::string password = req.target().substr(passPos, sepPos - passPos);
    std::string username = req.target().substr(sepPos + 6);
//    std::cout << username << ' ' << password;

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
//    response.body() = "sample body response";
//    PGconn* connection = PQconnectdb("host=localhost port=5432 dbname=mydb user=postgres password=1812");
    Database* db = Database::GetInstance("postgres", "1812");
    PGconn* connection = db->getConn("postgres");
    if(PQstatus(connection) == ConnStatusType::CONNECTION_OK){
        response.body() = "connected";
    } else{
        response.body() = PQerrorMessage(connection);
    }
    std::string query =("select (password) from users where login=\'" + username + "\';");
    PGresult* res = PQexec(connection, query.c_str());

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            boost::json::object session;
            session["status"] = "ok";
            response.body() = serialize(session);
        } else {
            boost::json::object session;
            session["status"] = "wrong password";
            response.body() = serialize(session);
        }
    } else{
        boost::json::object session;
        session["status"] = "wrong username";
        response.body() = serialize(session);
    }
    PQclear(res);

    return response;
}

http::response<http::string_body> post_registerResponse(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto sepPos = req.target().find("&user=");
    std::string password = req.target().substr(passPos, sepPos - passPos);
    std::string username = req.target().substr(sepPos + 6);

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database* db = Database::GetInstance("postgres", "1812");
    PGconn* connection = db->getConn("postgres");
    if(PQstatus(connection) == ConnStatusType::CONNECTION_OK){
        response.body() = "connected";
    } else{
        response.body() = PQerrorMessage(connection);
    }
    std::string query =("insert into users (login, password) values (\'" + username + "\', \'" + password + "\');");
    PGresult* res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_COMMAND_OK){
        boost::json::object session;
        session["status"] = "ok";
        response.body() = serialize(session);
    } else{
        boost::json::object session;
        session["status"] = "error";
        response.body() = serialize(session);
    }
    PQclear(res);
    return response;
}