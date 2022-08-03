#ifndef CLIENT_H
#define CLIENT_H

#include "contacts.hpp"
#include "request.hpp"
#include "user.hpp"
#include "active_conn.hpp"
#include "IO_strategy.hpp"
#include "text_io.hpp"

#include <map>
#include <vector>

#define MAX_MSG_LEN 1024

class Client {

public:
    Client(int t_port);
    
    bool ping();

    // Contacts commands
    std::vector<std::string> contact_get_contacts() const;

    bool contact_list();
    bool contact_search(std::string& t_cmd);
    bool contact_add_user(std::string& t_cmd);
    bool contact_remove_user(std::string& t_cmd);
    bool contact_available(std::string& t_cmd);

    // User commands
    bool user_set_username(std::string& t_username);
    bool user_set_password(std::string& t_password);

    bool user_register_user();
    bool user_login();

    std::string user_get_username() const;
    bool user_get_logged_in() const;

private:
    ActiveConn server_conn;
    Request req;
    
    User m_user;
    Contacts m_contacts;

    std::map<std::string, Request> m_contacts_request;
};

#endif