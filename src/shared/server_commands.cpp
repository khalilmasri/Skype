#include "server_commands.hpp"
#include "logger.hpp"

const std::unordered_map<std::string, ServerCommand::name>
    ServerCommand::m_commands{
        {"CREATE", Create},
        {"LOGIN", Login},
        {"LIST", List},
        {"SEARCH", Search},
        {"ADD", Add},
        {"REMOVE", Remove},
        {"PING", Ping},
        {"AVAILABLE", Available},
        {"EXIT", Exit},
        {"SEND", Send},
        {"CHAT", Chat},
        {"PENDING", Pending},
        {"REJECT", Reject},
        {"CONNECT", Connect},
        {"ACCEPT", Accept},
        {"HANGUP", Hangup},
        {"DELIVERED", Delivered},
    };
ServerCommand::name ServerCommand::get(const std::string &t_command) {
  try {
    return m_commands.at(t_command);

  } catch (...) {

    return None;
  }
}
bool ServerCommand::has_argument(name t_cmd) { return t_cmd < List; }

bool ServerCommand::has_zero_or_more_arguments(name t_cmd) {
  return t_cmd >= Pending;
}

std::string ServerCommand::to_string(name t_cmd) {

  for (auto it = m_commands.begin(); it != m_commands.end(); it++) {

    if (it->second == t_cmd)
      return it->first;
  }

  LOG_ERR("Attempted to convert a ServerCommand to string that does not exist");
  return std::string(); // return empty if fails to find.
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
