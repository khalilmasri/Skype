#ifndef ACTIVE_CONTROL_H
#define ACTIVE_CONTROL_H
#include "connection_strategy.hpp"


class ActiveControl : public ConnectionStrategy {

public:
  bool connect_socket(int t_socket, sockaddr_in t_addr) override;
  bool send(std::string &t_data) override;
  bool receive(std::string &t_data) override;

  ~ActiveControl() override;
};

#endif
