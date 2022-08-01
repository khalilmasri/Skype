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
  PassiveConn(int t_port ,IOStrategy* t_io) : Connection(t_port), m_io(t_io){};
  ~PassiveConn() {

    shutdown(get_socket(), SHUT_RDWR);
    m_poll.close_all(); // close all connected socket fds in poll;
    delete m_io;
  }

  bool bind_and_listen(const std::string &t_address);
  Request accept_connection() ;
  bool receive(Request &t_req);
  bool respond(Request &t_req);

private:
  std::string get_client_address(sockaddr_in t_address);
};

#endif // !PASSIVE_CONN_H
