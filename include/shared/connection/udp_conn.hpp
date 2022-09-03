#ifndef UDP_CONN_H
#define UDP_CONN_H

#include "connection.hpp"
#include "IO_strategy.hpp"

class UDPConn : public Connection {

  IOStrategy*    m_io;

  public:
  UDPConn(int t_port, IOStrategy* t_io);
  bool bind_socket(const std::string &t_address);

  bool receive(Request &t_req);
  bool respond(Request &t_req);

  private:
  std::string m_peer_address;
  std::string m_peer_port;

};

#endif
