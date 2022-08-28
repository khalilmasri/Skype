#include "chat_controllers.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "controller_utils.hpp"

Postgres ChatControllers::m_pg = Postgres();
const std::string ChatControllers::m_HEADER_DELIM = ":";
const std::string ChatControllers::m_HEADER_FIELDS_DELIM = ",";

void ChatControllers::send(std::string &t_arg, Request &t_req) {

  auto [recipient_id, message] = StringUtils::split_first(t_arg);

  User sender = m_pg.search_user_by(t_req.m_address, "address");
  User recipient = m_pg.search_user_by(recipient_id, "id");

  // TODO: needs error handling
  if (!sender.empty() && !recipient.empty()) {
    UserChat user_chat(sender, recipient, message);
    m_pg.add_user_chat(user_chat);
    ControllerUtils::set_request_reply(Reply::r_200, t_req);
      
  } else {
    ControllerUtils::set_request_reply(Reply::r_301, t_req); // not found
                                                             
  }
}


void ChatControllers::pending(std::string &t_arg, Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");
  User chats_with = m_pg.search_user_by(t_arg, "id");

  if (!user.empty() && !chats_with.empty()) {

    UserChats pending_chats = m_pg.list_user_pending_chats(user, chats_with);
    std::string response    = build_response(pending_chats);

    ControllerUtils::set_request_reply(Reply::r_201, std::move(response), t_req);

  } else {
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
  }

}


std::string ChatControllers::build_response(UserChats &t_user_chats) {

     std::string response ="0";
    std::string content;

    for(auto &chat : t_user_chats){
      std::string chat_str = chat.to_string();
      std::string chat_header =
        std::to_string(chat_str.size() - chat.text_length()) + UserChat::m_HEADER_DELIM;

      content += chat_header;
      content += chat_str;
      response += ChatControllers::m_HEADER_FIELDS_DELIM;
      response += std::to_string(content.size());
    }

    response +=  ChatControllers::m_HEADER_DELIM;
    response +=  content;

    return response;
}
