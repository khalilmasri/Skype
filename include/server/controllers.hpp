#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "reply.hpp"
#include "request.hpp"
#include "postgres.hpp"

struct Controllers {

  static void  create    (std::string &t_arg, Request &t_req);
  static void  login     (std::string &t_arg, Request &t_req);
  static void  search    (std::string &t_arg, Request &t_req);
  static void  add       (std::string &t_arg, Request &t_req){};
  static void  remove    (std::string &t_arg, Request &t_req){};
  static void  ping      (std::string &t_arg, Request &t_req){};
  static void  available (std::string &t_arg, Request &t_req){};
  static void  none      (std::string &t_arg, Request &t_req){};

  private:
    static Postgres m_pg;

    static bool is_empty(std::string &t_user, std::string &t_password, Request &t_req);

    static void set_request_reply(Reply::Code t_reply, Request &t_req);
};

#endif // !CONTROLLERS_H
      
