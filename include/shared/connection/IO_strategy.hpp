#ifndef IO_STRATEGY_H
#define IO_STRATEGY_H
#include <string>
#include "request.hpp"
#include "logger.hpp"


class IOStrategy {

public:
  virtual bool receive(Request &req) const = 0;
  virtual bool respond(Request &req) const = 0;

bool is_valid(int t_result, const char *t_msg) const {
  if (t_result <= 0) {
    LOG_ERR(t_msg);
  }
  return t_result >= 0;
}

  virtual ~IOStrategy(){};
};

#endif // !IO_STRATEGY_H
