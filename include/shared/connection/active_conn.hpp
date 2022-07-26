#ifndef ACTIVE_CONN_H
#define ACTIVE_CONN_H
#include "connection.hpp"
#include "IO_strategy.hpp"
#include <unistd.h>

class ActiveConn : public Connection {

  IOStrategy  *m_io;

public:
  ActiveConn(int t_port, IOStrategy *t_io): Connection(t_port), m_io(t_io){
  };

 ~ActiveConn() {
   close(get_socket());
    delete m_io;
}

  bool connect_socket(std::string &address);
  bool receive(std::string &t_data);
  bool respond(std::string &t_data);

};

#endif
