#ifndef PASSIVE_CONN_H
#define PASSIVE_CONN_H

#include "IO_strategy.hpp"
#include "connection_poll.hpp"
#include "connection.hpp"
#include "request.hpp"
#include <unordered_map>

class PassiveConn : public Connection {

  typedef std::unordered_map<int, std::string> AddressMap;

  IOStrategy*    m_io;
  ConnectionPoll m_poll;
  AddressMap     m_addresses;

public:
  PassiveConn(int t_port ,IOStrategy* t_io);
  ~PassiveConn();

  bool bind_and_listen(const std::string &t_address);
  Request accept_connection() ;
  bool receive(Request &t_req);
  bool respond(Request &t_req);
  void disconnect_client(Request &t_req);

private:
  std::string get_client_address(sockaddr_in t_address);
};

#endif // !PASSIVE_CONN_H
