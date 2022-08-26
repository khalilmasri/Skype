#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H

#include "active_conn.hpp"
#include "reply.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <QVector>
#include <QString>

class Contacts {

public:
    bool list(ActiveConn& t_conn, Request& t_req);
    bool search(ActiveConn& t_conn, Request& t_req);
    bool add_user(ActiveConn& t_conn, Request& t_req);
    bool remove_user(ActiveConn& t_conn, Request& t_req);
    bool available(ActiveConn& t_conn, Request& t_req);

    //contacts retrieval
    bool set_current_contact(std::string &t_current_contact);
    std::string get_current_contact();
    QVector<QString> display_contacts();

private:

    struct Details{
        int ID;
        std::string address;
        bool online;
    };

    std::unordered_map<std::string, struct Details> m_online_contacts;
    std::unordered_map<std::string, struct Details> m_old_contacts;

    std::vector<std::string> m_contacts;
    std::string m_current_contact;
    
    void update_contacts(std::string t_response);
    void pair_contact_details(std::string t_user);
    
    bool valid_response(Reply::Code t_c, std::string& t_res);
};

#endif
