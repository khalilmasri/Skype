#ifndef USER_H
#define USER_H

#include "active_conn.hpp"
#include "request.hpp"

#include <string>

#define MAX_MSG_LEN 1024

class User {

public:
    bool register_user(ActiveConn& t_conn, Request& t_req);
    bool login(ActiveConn& t_conn, Request& t_req);


    bool set_username(std::string& t_username);
    bool set_password(std::string& t_password);
    
    std::string get_username();
    bool get_logged_in();

private:
    std::string m_username;
    std::string m_password;

    bool m_logged_in = false;
    bool valid_response(Reply::Code t_code, std::string& t_res);
};

#endif