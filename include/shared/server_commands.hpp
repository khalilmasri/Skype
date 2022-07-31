#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <string>
#include <unordered_map>

struct ServerCommand {
  enum name {Create, Login, List, Search, Add, Remove, Ping, Available, Invalid};

  static name get(const std::string &t_command);

  private:
  const static std::unordered_map<std::string, name> m_commands;

};



#endif // !SERVER_COMMANDS_H
