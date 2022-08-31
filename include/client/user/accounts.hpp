#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include "active_conn.hpp"
#include "request.hpp"

#include <string>

class Accounts {

public:
    bool register_user(ActiveConn& t_conn, Request& t_req);
    bool login(ActiveConn& t_conn, Request& t_req);


    bool set_username(std::string& t_username);
    bool set_password(std::string& t_password);
    
    std::string get_username();
    std::string get_token();
    bool get_logged_in();
    int get_id();

private:
    std::string m_username;
    std::string m_password;
    std::string m_token;
    int m_id;

    bool m_logged_in = false;
    int  set_id(ActiveConn& t_conn, Request& t_req);
    void extract_token(std::string &t_response);
    bool valid_response(Reply::Code t_code, std::string& t_res);
};

#endif
