#ifndef ROUTER_H
#define ROUTER_H

#include "reply.hpp"
#include "request.hpp"
#include "server_commands.hpp"
#include "user_controllers.hpp"
#include <unordered_map>

class Router {
  typedef ServerCommand::name Command;
  typedef std::tuple<ServerCommand::name, std::string, std::string> CmdTuple;
  typedef void (*ControllerFunc)(std::string&, Request&);
  typedef std::unordered_map<ServerCommand::name, ControllerFunc> ControllersMap;

public:

  Router();
  void              route(Request &t_req);

private:
  ControllersMap    m_controllers;
  CmdTuple          parse(Request &t_req);
  bool              validate_argument(ServerCommand::name t_cmd, std::string &t_arg);
  bool              is_loggedin(ServerCommand::name t_cmd, std::string &t_token, Request &t_req);
  void              invalid_command(Request &t_req);
  bool              require_login(ServerCommand::name t_cmd);

};

#endif // !CONTROLLERS_H
