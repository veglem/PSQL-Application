//
// Created by vzhur on 17.05.2023.
//

#include "responceHeaders.h"
#include "urlDecode.h"

http::response<http::string_body> get_developer(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");
    auto namePos = req.target().find("&name=");
    std::string password;
    std::string username;
    std::string comp_name;
    if (namePos == -1) {
        password = req.target().substr(passPos, userPos - passPos);
        username = req.target().substr(userPos + 6);

    } else {
        password = req.target().substr(passPos, userPos - passPos);
        username = req.target().substr(userPos + 6, namePos - userPos - 6);
        comp_name = req.target().substr(namePos + 6);
        comp_name = url_Decode(comp_name);
    }

    if(username != "admin"){
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return  response;
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
            if (!comp_name.empty()){
                comp_name = "where full_name=\'" + comp_name + '\'';
            }
            query = "select d.email, d.xp_years, d.garde, p.pos_name, dt.email, full_name from developer d join developer_team dt on dt.id = d.team_id join position p on p.id = d.position_id " +
                    comp_name + ";";

            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
            for (int i = 0; i < PQntuples(res); ++i) {
                boost::json::object temp;
                for (int j = 0; j < PQnfields(res); ++j) {
                    if (j == 4){
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

http::response<http::string_body> post_add_developer(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");

    std::string password;
    std::string username;

    password = req.target().substr(passPos, userPos - passPos);
    username = req.target().substr(userPos + 6);

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
            //insert into developer (email, xp_years, garde, position_id, team_id, full_name) values ('email', 10, 'grade', (select id from position where pos_name='position'), (select id from developer_team where developer_team.email='email'), 'name');
            query = "insert into developer (email, xp_years, garde, position_id, team_id, full_name) values (\'";
            query += req_body.at("email").as_string().c_str();
            query += "\', \'";
            query += req_body.at("xp_years").as_string().c_str();
            query += "\', \'";
            query += req_body.at("grade").as_string().c_str();
            query += "\', (select id from position where pos_name=\'";
            query += req_body.at("position").as_string().c_str();
            query += "\'), (select id from developer_team where developer_team.email=\'";
            query += req_body.at("team_email").as_string().c_str();
            query += "\'), \'";
            query +=  req_body.at("full_name").as_string().c_str();
            query += "\');";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
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

http::response<http::string_body> post_add_dev_team(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");

    std::string password;
    std::string username;

    password = req.target().substr(passPos, userPos - passPos);
    username = req.target().substr(userPos + 6);

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
            query = "insert into developer_team (email) values (\'";
            query += req_body.at("email").as_string().c_str();
            query += "\');";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
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

http::response<http::string_body> put_developer(beast::error_code err, http::request<http::string_body>& req){
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
            boost::json::value req_body = boost::json::parse(req.body());
            //update developer set email='',
            //                     xp_years='',
            //                     garde='',
            //                     position_id=(select id from position where pos_name=''),
            //                     team_id=(select id from developer_team where developer_team.email=''),
            //                     full_name='' where full_name='';
            query = "update developer set email=\'";
            query += req_body.at("email").as_string().c_str();
            query += "\', xp_years=\'";
            query += req_body.at("xp_years").as_string().c_str();
            query += "\', garde=\'";
            query += req_body.at("grade").as_string().c_str();
            query += "\', position_id=(select id from position where pos_name=\'";
            query += req_body.at("position").as_string().c_str();
            query += "\'), team_id=(select id from developer_team where developer_team.email=\'";
            query += req_body.at("team_email").as_string().c_str();
            query += "\'), full_name=\'";
            query +=  req_body.at("full_name").as_string().c_str();
            query += "\' where full_name=\'" + comp_name + "\';";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
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

http::response<http::string_body> delete_developer(beast::error_code err, http::request<http::string_body>& req){
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

            //delete from developer where full_name='';
            query = "delete from developer where full_name=\'" + comp_name + "\';";

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

http::response<http::string_body> put_dev_team(beast::error_code err, http::request<http::string_body>& req){
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
            boost::json::value req_body = boost::json::parse(req.body());
            //update developer_team set email='' where email='';
            query = "update developer_team set email=\'";
            query += req_body.at("email").as_string().c_str();
            query += "\' where email=\'" + comp_name + "\';";

            PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK){
                boost::json::object session;
                session["status"] = "ok";
                response.body() = serialize(session);
            } else{
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

http::response<http::string_body> delete_dev_team(beast::error_code err, http::request<http::string_body>& req){
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

            //delete from developer_team where email='';
            query = "delete from developer_team where email=\'" + comp_name + "\';";

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

http::response<http::string_body> get_dev_team(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto sepPos = req.target().find("&user=");
    std::string password = req.target().substr(passPos, sepPos - passPos);
    std::string username = req.target().substr(sepPos + 6);


    if(username != "admin"){
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return  response;
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

            query = "select email from developer_team;";

            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
            for (int i = 0; i < PQntuples(res); ++i) {
                boost::json::object temp;
                for (int j = 0; j < PQnfields(res); ++j) {
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

http::response<http::string_body> post_search_user(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto userPos = req.target().find("&user=");
    auto namePos = req.target().find("&name=");
    std::string password;
    std::string username;
    std::string comp_name;
    if (namePos == -1) {
        password = req.target().substr(passPos, userPos - passPos);
        username = req.target().substr(userPos + 6);

    } else {
        password = req.target().substr(passPos, userPos - passPos);
        username = req.target().substr(userPos + 6, namePos - userPos - 6);
        comp_name = req.target().substr(namePos + 6);
        comp_name = url_Decode(comp_name);
    }

    if(username != "admin"){
        http::response<http::string_body> response{http::status::forbidden, req.version()};
        response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(req.keep_alive());
        return  response;
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
            //select login from users where login like '%%';

            query = "select login, password from users where login like \'%" +
                    comp_name + "%\';";
            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
            for (int i = 0; i < PQntuples(res); ++i) {
                boost::json::object temp;
                for (int j = 0; j < PQnfields(res); ++j) {

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