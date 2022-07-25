#ifndef IO_STRATEGY_H
#define IO_STRATEGY_H
#include <string>

class IOStrategy {

public:
  virtual bool receive(int t_socket, std::string &t_data) const = 0;
  virtual bool respond(int t_socket, std::string &t_data) const = 0;

  virtual ~IOStrategy(){};
};

#endif // !IO_STRATEGY_H
