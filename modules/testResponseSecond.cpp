//
// Created by vzhur on 12.05.2023.
//

#include "testResponseSecond.h"

http::response<http::string_body> get_testResponseSecond(beast::error_code err, http::request<http::string_body>& req){
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
    PGresult* res = PQexec(connection, "select * from developer");
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