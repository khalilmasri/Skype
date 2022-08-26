#include "server_commands.hpp"
#include <unordered_map>

const std::unordered_map<std::string, ServerCommand::name>
    ServerCommand::m_commands{
        {"CREATE", Create}, {"LOGIN", Login},
        {"LIST", List},     {"SEARCH", Search},
        {"ADD", Add},       {"REMOVE", Remove},
        {"PING", Ping},     {"AVAILABLE", Available},
        {"EXIT", Exit},     {"SEND", Send},
        {"RECEIVE", Receive}
    };
ServerCommand::name ServerCommand::get(const std::string &t_command) {
  try {

    return m_commands.at(t_command);
  } catch (...) {

    return None;
  }
}

bool ServerCommand::has_argument(name t_cmd) {
  return t_cmd < List ? true : false;
}

int ServerCommand::argument_count(name t_cmd) {
  if (t_cmd >= List) {
    return 0;
  }

  else if (t_cmd < List && t_cmd >= Search) {
    return 1;
  }

  else {
    return 2;
  }
}
