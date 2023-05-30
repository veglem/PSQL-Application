//
// Created by vzhur on 20.05.2023.
//
#include "responceHeaders.h"
#include "urlDecode.h"

http::response<http::string_body> get_position(beast::error_code err, http::request<http::string_body> &req) {
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");

    std::string password;
    std::string username;

    password = req.target().substr(passPos, userPos - passPos);
    username = req.target().substr(userPos + 6);

    if (username != "admin") {
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return response;
    }


    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database *db = Database::GetInstance("postgres", "1812");
    PGconn *connection = db->getConn("postgres");
    if (PQstatus(connection) == ConnStatusType::CONNECTION_OK) {
        response.body() = "connected";
    } else {
        response.body() = PQerrorMessage(connection);
        return response;
    }
    std::string query = ("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            //select d.email, d.xp_years, d.garde, p.pos_name, dt.email, full_name from developer d join developer_team dt on dt.id = d.team_id join position p on p.id = d.position_id where full_name='Пупков Кирилл Валерьевич';

            query = "select pos_name from position;";

            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
            for (int i = 0; i < PQntuples(res); ++i) {
                boost::json::object temp;
                for (int j = 0; j < PQnfields(res); ++j) {
                    if (j == 4) {
                        temp["team_email"] = PQgetvalue(res, i, j);
                        continue;
                    }
                    temp[PQfname(res, j)] = PQgetvalue(res, i, j);
                }
                res_json.push_back(temp);
            }
            response.body() = serialize(res_json);
        } else {
            boost::json::object session;
            session["status"] = "wrong password";
            response.body() = serialize(session);
        }
    } else {
        boost::json::object session;
        session["status"] = "wrong username";
        response.body() = serialize(session);
    }
    return response;
}


http::response<http::string_body> put_position(beast::error_code err, http::request<http::string_body> &req) {
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");
    auto namePos = req.target().find("&name=");
    std::string password;
    std::string username;
    std::string comp_name;

    password = req.target().substr(passPos, userPos - passPos);
    username = req.target().substr(userPos + 6, namePos - userPos - 6);
    comp_name = req.target().substr(namePos + 6);
    comp_name = url_Decode(comp_name);

    if (username != "admin") {
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return response;
    }

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database *db = Database::GetInstance("postgres", "1812");
    PGconn *connection = db->getConn("postgres");
    if (PQstatus(connection) == ConnStatusType::CONNECTION_OK) {
        response.body() = "connected";
    } else {
        response.body() = PQerrorMessage(connection);
        return response;
    }
    std::string query = ("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            boost::json::value req_body = boost::json::parse(req.body());
            //update position set pos_name='' where pos_name='';
            query = "update position set pos_name=\'";
            query += req_body.at("pos_name").as_string().c_str();
            query += "\' where pos_name=\'" + comp_name + "\';";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK) {
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else {
                boost::json::object session;
                session["status"] = PQresultErrorMessage(res);
                response.body() = serialize(session);
            }

        } else {
            boost::json::object session;
            session["status"] = "wrong password";
            response.body() = serialize(session);
        }
    } else {
        boost::json::object session;
        session["status"] = "wrong username";
        response.body() = serialize(session);
    }
    return response;

}

http::response<http::string_body> delete_position(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");
    auto namePos = req.target().find("&name=");
    std::string password;
    std::string username;
    std::string comp_name;

    password = req.target().substr(passPos, userPos - passPos);
    username = req.target().substr(userPos + 6, namePos - userPos - 6);
    comp_name = req.target().substr(namePos + 6);
    comp_name = url_Decode(comp_name);

    if (username != "admin") {
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return response;
    }

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database *db = Database::GetInstance("postgres", "1812");
    PGconn *connection = db->getConn("postgres");
    if (PQstatus(connection) == ConnStatusType::CONNECTION_OK) {
        response.body() = "connected";
    } else {
        response.body() = PQerrorMessage(connection);
        return response;
    }
    std::string query =("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);

            //delete from position where pos_name='';
            query = "delete from position where pos_name=\'" + comp_name + "\';";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
                boost::json::object session;
                session["status"] = "error";
                response.body() = serialize(session);
            }

        } else {
            boost::json::object session;
            session["status"] = "wrong password";
            response.body() = serialize(session);
        }
    } else {
        boost::json::object session;
        session["status"] = "wrong username";
        response.body() = serialize(session);
    }
    return response;
}

http::response<http::string_body> add_position(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto sepPos = req.target().find("&user=");
    std::string password = req.target().substr(passPos, sepPos - passPos);
    std::string username = req.target().substr(sepPos + 6);

    if (username != "admin") {
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return response;
    }

    http::response<http::string_body> response{http::status::ok, req.version()};
    response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(http::field::content_type, "application/json");
    response.keep_alive(req.keep_alive());
    Database *db = Database::GetInstance("postgres", "1812");
    PGconn *connection = db->getConn("postgres");
    if (PQstatus(connection) == ConnStatusType::CONNECTION_OK) {
        response.body() = "connected";
    } else {
        response.body() = PQerrorMessage(connection);
        return response;
    }
    std::string query =("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            boost::json::value req_body = boost::json::parse(req.body());
            query = "insert into position (pos_name) values (\'";
            query += req_body.at("pos_name").as_string().c_str();
            query += "\');";
            //insert into position (pos_name) values ('');
            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
                boost::json::object session;
                session["status"] = "error";
                response.body() = serialize(session);
            }

        } else {
            boost::json::object session;
            session["status"] = "wrong password";
            response.body() = serialize(session);
        }
    } else {
        boost::json::object session;
        session["status"] = "wrong username";
        response.body() = serialize(session);
    }
    return response;
}