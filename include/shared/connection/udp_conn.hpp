#ifndef UDP_CONN_H
#define UDP_CONN_H

#include "connection.hpp"
#include "IO_strategy.hpp"

class UDPConn : public Connection {

  IOStrategy*    m_io;

  public:
  UDPConn(int t_port, IOStrategy* t_io);
  explicit UDPConn(IOStrategy* t_io);
  auto bind_socket(const std::string &t_address) -> bool;

  void set_strategy(IOStrategy *t_io);

  auto receive(Request &t_req) -> bool;
  auto respond(Request &t_req) -> bool;

  private:
  std::string m_peer_address;
  std::string m_peer_port;

  // recvfrom will timeout within 1 second
  const inline static int m_RECEIVE_TIMEOUT = 10;

};

#endif
