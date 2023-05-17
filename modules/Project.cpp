#include "responceHeaders.h"
#include "urlDecode.h"

http::response<http::string_body> get_project(beast::error_code err, http::request<http::string_body> &req) {
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
    std::string query = ("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);

            query = "select project_name, project.description, tech_task, o.status, o.price, o.deadline, o.start_date from project  join client_company cc on project.company_id = cc.id full join orders o on project.id = o.project_id where cc.company_name = \'" +
                    comp_name + "\';";

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

http::response<http::string_body> post_add_project(beast::error_code err, http::request<http::string_body> &req) {
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
    std::string query =("select (password) from users where login=\'" + username + "\';");
    PGresult *res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) != 0) {
        std::string db_pass = PQgetvalue(res, 0, 0);
        if (db_pass == password) {
            PQclear(res);
            boost::json::value req_body = boost::json::parse(req.body());
            //insert into project (company_id, project_name, description, tech_task) values ((select id from client_company where company_name=''), 'a', 'b', 'c');
            query = "insert into project (company_id, project_name, description, tech_task) values ((select id from client_company where company_name=\'";
            query += comp_name;
            query += "\'), \'";
            query += req_body.at("project_name").as_string().c_str();
            query += "\', \'";
            query += req_body.at("description").as_string().c_str();
            query += "\', \'";
            query += req_body.at("tech_task").as_string().c_str();
            query += "\');";
            std::string query2;
            //insert into orders (team_id, project_id, status, price, deadline, start_date) values (1, (select id from project where project_name=''), 0, 1000, 'YYYY-MM-DD', 'YYYY-MM-DD');
            query2 = "insert into orders (team_id, project_id, status, price, deadline, start_date) values (" + std::to_string(std::rand()%4 + 1);
            query2 += ", (select id from project where project_name='";
            query2 += req_body.at("project_name").as_string().c_str();
            query2 += "\'), false, " + std::to_string(req_body.at("tech_task").as_string().size() * 1000) + ", \'";
            auto a = std::time(nullptr);
            auto now = std::localtime(&a);
            query2 += to_string(now->tm_year + 1900) + '-' + to_string(now->tm_mon) + '-' + to_string(now->tm_mday) + "\', \'";
            a += 2.592e+6;
            now = std::localtime(&a);
            query2 += to_string(now->tm_year + 1900) + '-' + to_string(now->tm_mon) + '-' + to_string(now->tm_mday) + "\');";
            query += query2;
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
