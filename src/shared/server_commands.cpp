#include "server_commands.hpp"
#include <unordered_map>

const std::unordered_map<std::string, ServerCommand::name>
    ServerCommand::m_commands{
        {"CREATE", Create}, {"LOGIN", Login},
        {"LIST", List},     {"SEARCH", Search},
        {"ADD", Add},       {"REMOVE", Remove},
        {"PING", Ping},     {"AVAILABLE", Available},
    };

ServerCommand::name ServerCommand::get(const std::string &t_command) {
  try {
    return m_commands.at(t_command);
  } catch (...) {

    return None;
  }
}
