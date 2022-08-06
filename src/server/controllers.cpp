#include "controllers.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include "supress_unused_params_warnings.hpp"
#include "text_data.hpp"
#include <iostream>

#define CONTACT_DELIM " "

Postgres Controllers::m_pg = Postgres();

void Controllers::list(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if (!user.empty()) {
    Users contacts = m_pg.list_user_contacts(
        user); // TODO: what if user doesnt have contacts
               //
    std::string reply_msg;

    for (auto &contact : contacts) {
      reply_msg += contact.to_string() + CONTACT_DELIM;
    }

    reply_msg.pop_back(); // remove last delim

    set_request_reply(Reply::r_201, std::move(reply_msg), t_req);

  } else {
    set_request_reply(Reply::r_500, t_req);
  }
}

void Controllers::create(std::string &t_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(t_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user(0, username, password, false, t_req.m_address);
  bool result = m_pg.add_user(user);

  if (result) {
    set_request_reply(Reply::r_200, t_req);

  } else {
    set_request_reply(Reply::r_300, t_req);
  }
};

void Controllers::login(std::string &t_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(t_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user = m_pg.search_user_by(username, "username");

  if (!user.empty() && user.password() == password) {
    login_user(user, t_req);
  } else {
    set_request_reply(Reply::r_300, t_req);
  }
}

void Controllers::search(std::string &t_username, Request &t_req) {

  User user = m_pg.search_user_by(t_username, "username");

  if (!user.empty()) {
    set_request_reply(Reply::r_201, user.to_string(), t_req);
    std::string reply = Reply::append_message(Reply::r_201, user.to_string());

  } else {

    set_request_reply(Reply::r_301, t_req);
  }
}

void Controllers::add(std::string &t_contact_username, Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");
  User contact = m_pg.search_user_by(t_contact_username, "username");

  if (!user.empty() && !contact.empty()) {

    bool res = m_pg.add_user_contact(user, contact);
    set_request_reply(res, t_req);

  } else {
    set_request_reply(Reply::r_301, t_req);
  }
}

void Controllers::remove(std::string &t_contact_username, Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");
  User contact = m_pg.search_user_by(t_contact_username, "username");

  if (!user.empty() && !contact.empty()) {

    bool res = m_pg.remove_user_contact(user, contact);
    set_request_reply(res, t_req);

  } else {
    set_request_reply(Reply::r_301, t_req);
  }
}

void Controllers::ping(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if (user.empty()) {
    set_request_reply(Reply::r_301, t_req); // didn't find a user. return not found

  } else if (!user.online()) {
    set_request_reply(Reply::r_202, t_req); // user is not online. Ask to login again.

  } else {
    set_request_reply(Reply::r_200, t_req); // user is online. return OK
  }
}

void Controllers::available(std::string &t_username, Request &t_req) {

  User user = m_pg.search_user_by(t_username, "username");

  if (user.empty()) {
    set_request_reply(Reply::r_301, t_req); // not found
  } 

  else if(!user.online()){
    set_request_reply(Reply::r_300, t_req); // not online
  }

  else {
    set_request_reply(Reply::r_201, user.to_string(), t_req);
  }
}

void Controllers::none(std::string &_, Request &t_req) {

  UNUSED_PARAMS(_);
  set_request_reply(Reply::r_501, t_req); // invalid command
}

bool Controllers::ip_exists(Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if(user.empty()){
    set_request_reply(Reply::r_202, t_req);
  }

  return !user.empty();
}


/**************** PRIVATE ****************/

void Controllers::login_user(User &t_user, Request &t_req) {

  t_user.update("true", User::Online);
  t_user.update(t_req.m_address, User::Address);

  bool res = m_pg.update(t_user);

  set_request_reply(res, t_req);
}

bool Controllers::is_empty(std::string &t_user, std::string &t_password,
                           Request &t_req) {

  bool res = t_user.empty() || t_password.empty();

  if (res) {
    set_request_reply(Reply::r_501, t_req);
  }

  return res;
}

void Controllers::set_request_reply(Reply::Code t_reply, Request &t_req) {
  std::string reply = Reply::get_message(t_reply);
  t_req.set_data(new TextData(reply));
}

void Controllers::set_request_reply(Reply::Code t_reply, std::string &&t_msg,
                                    Request &t_req) {
  std::string reply = Reply::append_message(t_reply, t_msg);
  t_req.set_data(new TextData(reply));
}

void Controllers::set_request_reply(bool t_valid, Request &t_req) {
  if (t_valid) {
    set_request_reply(Reply::r_200, t_req);

  } else {
    set_request_reply(Reply::r_500, t_req);
  }
}
