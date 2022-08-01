#ifndef ROUTER_H
#define ROUTER_H

#include "reply.hpp"
#include "request.hpp"
#include "server_commands.hpp"

class Router {
  typedef ServerCommand::name Command;

public:
  Router(){};

  bool              route(Request &t_req);
  Command           command() const;
  const std::string arguments() const;

private:
  Command           m_command = ServerCommand::None;
  Reply::code       m_reply = Reply::None;
  std::string       m_arguments;

  void parse(Request &t_req);
};

#endif // !CONTROLLERS_H
