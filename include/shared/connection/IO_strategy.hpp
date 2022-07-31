#ifndef IO_STRATEGY_H
#define IO_STRATEGY_H
#include <string>
#include "request.hpp"


class IOStrategy {

public:
  virtual bool receive(Request &req) const = 0;
  virtual bool respond(Request &req) const = 0;

  virtual ~IOStrategy(){};
};

#endif // !IO_STRATEGY_H
