#ifndef CONTROLLER_UTILS
#define CONTROLLER_UTILS

#include "reply.hpp"
#include "request.hpp"

struct ControllerUtils{

  static void  set_request_reply(Reply::Code t_reply, Request &t_req);
  static void  set_request_reply(Reply::Code t_reply, std::string &&t_msg, Request &t_req);
  static void  set_request_reply(bool t_valid, Request &t_req);
  static void  set_request_reply(bool t_valid, std::string &&t_msg, Request &t_req);
};

#endif // !CONTROLLER_UTILS
