#ifndef CLIENT_H
#define CLIENT_H

#include "contacts.hpp"
#include "user.hpp"
#include "active_conn.hpp"
#include "IO_strategy.hpp"
#include "text_io.hpp"

#define MAX_MSG_LEN 1024

class Client {

public:
    Client(int port);
    bool ping();

private:
    ActiveConn server_conn;
    
    User m_user;
    Contacts m_contacts;

};

#endif