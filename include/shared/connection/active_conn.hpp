#ifndef ACTIVE_CONN_H
#define ACTIVE_CONN_H
#include "connection.hpp"
#include "IO_strategy.hpp"
#include "request.hpp"
#include <unistd.h>

class ActiveConn : public Connection {

  IOStrategy  *m_io;

public:
  ActiveConn() {};
  ActiveConn(int t_port, IOStrategy *t_io): Connection(t_port), m_io(t_io){
  };

 ~ActiveConn() {
   close(get_socket());
    delete m_io;
}

  Request connect_socket(std::string &address);
  bool receive(Request &t_req);
  bool respond(Request &t_req);
};

#endif
