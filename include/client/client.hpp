#ifndef CLIENT_H
#define CLIENT_H

#include "active_conn.hpp"
#include "IO_strategy.hpp"
#include "text_io.hpp"

#define MAX_MSG_LEN 1024

class Client {

public:
    Client(int t_port);
    
    bool ping();

    bool server_send_request(Request&);

private:
    // Server connection
    ActiveConn m_server_conn;
    Request m_server_req;

    PassiveConn m_incoming;
    std::string my_ip = "127.0.0.1";
    // More connections with other clients
};

#endif
