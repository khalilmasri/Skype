#ifndef ROUTER_H
#define ROUTER_H

#include "reply.hpp"
#include "request.hpp"
#include "server_commands.hpp"
#include "controllers.hpp"
#include <unordered_map>

class Router {
  typedef ServerCommand::name Command;
  typedef std::tuple<ServerCommand::name, std::string> CmdTuple;
  typedef void (*ControllerFunc)(std::string&, Request&);
  typedef std::unordered_map<ServerCommand::name, ControllerFunc> ControllersMap;

public:

  Router();
  void              route(Request &t_req);

private:
  ControllersMap    m_controllers;
  CmdTuple          parse(Request &t_req);

};

#endif // !CONTROLLERS_H
