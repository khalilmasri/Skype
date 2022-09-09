#ifndef SERVER_H
#define SERVER_H

#include "passive_conn.hpp"
#include "text_io.hpp"
#include "router.hpp"
#include "udp_conn.hpp"
#include <string>

#define SERVER_ADDRESS "127.0.0.1"
#define UDP_PORT 7000

class Server {

public:
  Server(int t_port);
  void main_loop();
  void spawn_udp_listener();

private:
  PassiveConn m_conn;
  UDPConn     m_udp;
  std::string m_address;
  Router m_router;

  void accept_connection();
  void  disconnect_on_client_request(Request &t_req);
  void disconnect_client_on_failure(Request &t_req);

  void udp_worker();
};

#endif
