#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H

#include "active_conn.hpp"
#include "reply.hpp"

#include <iostream>
#include <string>
#include <vector>


#define MAX_MSG_LEN 1024 

class Contacts {

public:
    bool list(ActiveConn& t_conn, Request& t_req);
    bool search(ActiveConn& t_conn, Request& t_req);
    bool add_user(ActiveConn& t_conn, Request& t_req);
    bool remove_user(ActiveConn& t_conn, Request& t_req);
    bool available(ActiveConn& t_conn, Request& t_req);

    std::vector<std::string> get_contacts() const;

private:
    std::vector<std::string> m_contacts;
    bool valid_response(Reply::Code t_c, std::string& t_res);
};

#endif