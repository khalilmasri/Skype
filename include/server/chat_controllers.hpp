#ifndef CHAT_CONTROLLERS_H
#define CHAT_CONTROLLERS_H

#include "request.hpp"
#include "postgres.hpp"

struct ChatControllers {
  static void  send        (std::string &t_arg, Request &t_req);
  static void  pending     (std::string &t_arg, Request &t_req);
  static void  chat        (std::string &t_arg, Request &t_req);
  static void delivered     (std::string &t_arg, Request &t_req);

  private:
  static Postgres m_pg;
  static const std::string m_HEADER_DELIM;
  static const std::string m_HEADER_FIELDS_DELIM;

  static void        get_chats (std::string &t_arg, Request &t_req, const bool t_pending);
  static std::string build_response(UserChats &t_user_chats);
};

#endif
