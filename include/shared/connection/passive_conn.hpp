#ifndef PASSIVE_CONN_H
#define PASSIVE_CONN_H
#include "IO_strategy.hpp"
#include "connection.hpp"
#include "connection_poll.hpp"

class PassiveConn : public Connection {

  IOStrategy*    m_io;
  ConnectionPoll m_poll;
  int            m_client_socket;

public:
  PassiveConn() {};
  PassiveConn(int t_port ,IOStrategy* t_io) : Connection(t_port), m_io(t_io), m_client_socket(-1){};

  ~PassiveConn() {
    shutdown(get_socket(), SHUT_RDWR);
    m_poll.close_all(); // close all connected socket fds in poll;
    delete m_io;
  }

  bool bind_and_listen(const std::string &t_address);
  bool accept_connection() ;
  bool receive(std::string &t_data);
  bool respond(std::string &t_data);
};

#endif // !PASSIVE_CONN_H
