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
    bool remove_user(std::string& t_cmd);
    bool available(ActiveConn& t_conn, Request& t_req);

    //contacts retrieval
    std::vector<std::string> get_contacts() const;
    void set_current_contact(std::string &t_current_contact);
    std::string get_current_contact();

private:
    std::vector<std::string> m_contacts;
    std::string m_current_contact;
    bool valid_response(Reply::Code t_c, std::string& t_res);
};

#endif