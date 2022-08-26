#ifndef CHAT_CONTROLLERS_H
#define CHAT_CONTROLLERS_H

#include "reply.hpp"
#include "request.hpp"
#include "postgres.hpp"

struct ChatControllers {
  static void  send        (std::string &t_arg, Request &t_req);
  static void  receive     (std::string &t_arg, Request &t_req);

  private:
  static Postgres m_pg;

};



#endif

