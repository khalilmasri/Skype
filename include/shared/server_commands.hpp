#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <string>
#include <unordered_map>

struct ServerCommand {
  enum name {

    /* two argument */
    Create = 0,
    Login = 1,
    Send = 2,
    Connect = 3,
    Accept = 4,

    /* one argument */
    Search = 10,
    Add = 11,
    Remove = 12,
    Available = 13,
    Reject = 14,
  
    /* no argument */
    List = 20,
    Ping = 21,
    Exit = 23,
    None = 24,
    Hangup = 25,

    /* may have 0 or more argument */
    Pending = 30, 
    Chat = 32, 
    Delivered = 33,
  };

  static name get(const std::string &t_command);
  static bool has_zero_or_more_arguments(name t_cmd);
  static bool has_argument(name t_cmd);
  static std::string to_string(name t_cmd);
  static int  argument_count(name t_cmd);

private:
  const static std::unordered_map<std::string, name> m_commands;
};

#endif // !SERVER_COMMANDS_H
