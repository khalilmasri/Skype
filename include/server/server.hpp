#ifndef SERVER_H
#define SERVER_H

#include "passive_conn.hpp"
#include "text_io.hpp"
#include "router.hpp"
#include <string>

#define SERVER_ADDRESS "127.0.0.1"

class Server {

public:
  Server(int t_port);
  void main_loop();

private:
  PassiveConn m_conn;
  std::string m_address;
  Router m_router;

  void accept_connection();
  void disconnect_client_on_failure(Request &t_req);
};

#endif
