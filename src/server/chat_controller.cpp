#include "chat_controllers.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

Postgres ChatControllers::m_pg = Postgres();

void ChatControllers::send(std::string &t_arg, Request &t_req) {

  auto [recipient_id, message] = StringUtils::split_first(t_arg);

  User sender = m_pg.search_user_by(t_req.m_address, "address");
  User recipient = m_pg.search_user_by(recipient_id, "id");

  // TODO: needs error handling
  if (!sender.empty()) {
    UserChat user_chat(sender, recipient, message);
    m_pg.add_user_chat(user_chat);
    std::string reply = Reply::get_message(Reply::r_200);
    t_req.set_data(new TextData(reply));


  } else {
    std::string reply = Reply::get_message(Reply::r_500);
    t_req.set_data(new TextData(reply));
  }
}
