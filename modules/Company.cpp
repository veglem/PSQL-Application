//
// Created by vzhur on 15.05.2023.
//
#include "responceHeaders.h"
#include "urlDecode.h"


http::response<http::string_body> get_company(beast::error_code err, http::request<http::string_body> &req) {
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
//    std::cout << username << ' ' << password << ' ' << comp_name << ' ';
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

            if (!comp_name.empty()){
                comp_name = " and company_name=\'" + comp_name + "\'";
            }
            query = "select email, description, company_name from client_company join users_and_companies uac on client_company.id = uac.id_company join users u on uac.id_user = u.id where u.login=\'" +
                    username + "\'" + comp_name + ";";
            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
            for(int i = 0 ; i < PQntuples(res); ++i){
                boost::json::object temp;
                for(int j = 0; j < PQnfields(res); ++j){
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

http::response<http::string_body> post_add_company(beast::error_code err, http::request<http::string_body>& req){
    auto passPos = req.target().find("?auth") + 6;
    auto sepPos = req.target().find("&user=");
    std::string password = req.target().substr(passPos, sepPos - passPos);
    std::string username = req.target().substr(sepPos + 6);

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
            query = "insert into client_company (email, description, company_name) values (\'";
            query += req_body.at("email").as_string().c_str();
            query += "\', \'";
            query += req_body.at("description").as_string().c_str();
            query += "\', \'";
            query += req_body.at("company_name").as_string().c_str();
            query += "\');";
            //insert into users_and_companies (id_user, id_company) values ((select id from users where login=''), (select id from client_company where company_name=''));
            query += "insert into users_and_companies (id_user, id_company) values ((select id from users where login=\'" + username + "\'), (select id from client_company where company_name=\'";
            query += req_body.at("company_name").as_string().c_str();
            query += "\'));";
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

http::response<http::string_body> put_change_company(beast::error_code err, http::request<http::string_body>& req){
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
    boost::json::string query ="select (password) from users where login=\'";
    query += username + "\';";
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            //update orders set status='',
            //                  price='',
            //                  deadline='',
            //                  start_date='' where project_id=(select id from project where project_name='');
            boost::json::value req_body = boost::json::parse(req.body());
            query = "update client_company set email='";
            query += req_body.at("email").as_string();
            query +="',\ndescription='" ;
            query += req_body.at("description").as_string();
            query += "',\n company_name='";
            query += req_body.at("company_name").as_string().c_str();
            query += "' where company_name ='";
            query += comp_name;
            query += "';";


            res = PQexec(connection, query.c_str());
            boost::json::array res_json;
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

http::response<http::string_body> del_company(beast::error_code err, http::request<http::string_body>& req){
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
    boost::json::string query ="select (password) from users where login=\'";
    query += username + "\';";
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            query = "delete from users_and_companies where id_company=(select id from client_company where company_name='" + comp_name + "');";
            query += "delete from client_company where company_name='" + comp_name +"';";
            res = PQexec(connection, query.c_str());

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