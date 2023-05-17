//
// Created by vzhur on 06.05.2023.
//

#include "testResponce.h"
#include "Database.h"

http::response<http::string_body> get_testResponse(beast::error_code err, http::request<http::string_body>& req){
    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database* db = Database::GetInstance("user_can_create", "1812");
    PGconn* connection = db->getConn("user_can_create");
    if(PQstatus(connection) == ConnStatusType::CONNECTION_OK){
        response.body() = "connected";
    } else{
        response.body() = PQerrorMessage(connection);
    }
    PGresult* res = PQexec(connection, "select * from client_company");
    boost::json::array res_json;
    for(int i = 0 ; i < PQntuples(res); ++i){
        boost::json::object temp;
        for(int j = 0; j < PQnfields(res); ++j){
            temp[PQfname(res, j)] = PQgetvalue(res, i, j);
        }
        res_json.push_back(temp);
    }
    response.body() = serialize(res_json);
    return response;
}