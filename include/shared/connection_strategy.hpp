#ifndef CONNECTION_STRATEGY_H
#define CONNECTION_STRATEGY_H
#include <string>
#include <netinet/in.h> /* sockeaddr_in */

class ConnectionStrategy {

public:
  virtual bool connect_socket(int t_socket, sockaddr_in t_addr) = 0;
  virtual bool send(std::string &t_data) = 0;
  virtual bool receive(std::string &t_data) = 0;

   virtual ~ConnectionStrategy() = 0;
};

#endif // !CONNECTION_STRATEGY_H
