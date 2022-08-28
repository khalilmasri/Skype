#include "chat_controllers.hpp"
#include "controller_utils.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include <iostream>

Postgres ChatControllers::m_pg = Postgres();
const std::string ChatControllers::m_HEADER_DELIM = ":";
const std::string ChatControllers::m_HEADER_FIELDS_DELIM = ",";

/*** PUBLIC **/

void ChatControllers::send(std::string &t_arg, Request &t_req) {

  auto [recipient_id, message] = StringUtils::split_first(t_arg);

  if(message.empty()){
    ControllerUtils::set_request_reply(Reply::r_502, t_req);
  }

  User sender = m_pg.search_user_by(t_req.m_address, "address");
  User recipient = m_pg.search_user_by(recipient_id, "id");
  
  if (!sender.empty() && !recipient.empty()) {

    UserChat user_chat(sender, recipient, message);
    m_pg.add_user_chat(user_chat);
    ControllerUtils::set_request_reply(Reply::r_200, t_req);

  } else {
    ControllerUtils::set_request_reply(Reply::r_303, t_req); // not found
  }
}

void ChatControllers::pending(std::string &t_arg, Request &t_req) {
  bool pending = true;
  get_chats(t_arg, t_req, pending);
}


void ChatControllers::chat(std::string &t_arg, Request &t_req) {
  bool not_pending = false;
  get_chats(t_arg, t_req, not_pending);

}

/** PRIVATE **/

void  ChatControllers::get_chats (std::string &t_arg, Request &t_req, const bool t_pending){
  User user = m_pg.search_user_by(t_req.m_address, "address");


  if (user.empty()) {
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
    return;
  }

  UserChats chats;

  if (t_arg.empty()) {
    chats = m_pg.list_user_chats(user, t_pending); // true for pending
                                                         
  } else {
    User chats_with = m_pg.search_user_by(t_arg, "id");

    if(chats_with.empty()){
      ControllerUtils::set_request_reply(Reply::r_304, t_req); // Sender not found
      return;
    }

    chats = m_pg.list_user_chats(user, t_pending, chats_with);
  }

  std::string response = build_response(chats);
  ControllerUtils::set_request_reply(Reply::r_201, std::move(response), t_req);
}


std::string ChatControllers::build_response(UserChats &t_user_chats) {

  std::string response = "0";
  std::string content;

  for (auto &chat : t_user_chats) {
    std::string chat_str = chat.to_string();
    std::string chat_header =
        std::to_string(chat_str.size() - chat.text_length()) + UserChat::m_HEADER_DELIM;

    content += chat_header;
    content += chat_str;
    response += ChatControllers::m_HEADER_FIELDS_DELIM;
    response += std::to_string(content.size());
  }

  response += ChatControllers::m_HEADER_DELIM;
  response += content;

  return response;
}
