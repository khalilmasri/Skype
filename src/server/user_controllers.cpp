#include "user_controllers.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include "supress_unused_params_warnings.hpp"
#include "text_data.hpp"
#include "controller_utils.hpp"
#include <iostream>

#define CONTACT_DELIM " "

Postgres UserControllers::m_pg = Postgres();

void UserControllers::list(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if (!user.empty()) {
    Users contacts = m_pg.list_user_contacts( user); 
    list_contacts(contacts, t_req);
     
  } else {
    ControllerUtils::set_request_reply(Reply::r_500, t_req);
  }
}

void UserControllers::create(std::string &t_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(t_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user(0, username, password, false, t_req.m_address);
  bool result = m_pg.add_user(user);

  if (result) {
    ControllerUtils::set_request_reply(Reply::r_200, t_req);

  } else {
    ControllerUtils::set_request_reply(Reply::r_300, t_req);
  }
};

void UserControllers::login(std::string &t_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(t_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user = m_pg.search_user_by(username, "username");

  if (!user.empty() && user.password() == password) {
    login_user(user, t_req);
  } else {
    ControllerUtils::set_request_reply(Reply::r_300, t_req);
  }
}

void UserControllers::search(std::string &t_username, Request &t_req) {

  User user = m_pg.search_user_by(t_username, "username");

  if (!user.empty()) {
    user.remove_password(); // do not return user password to client
    ControllerUtils::set_request_reply(Reply::r_201, user.to_string(), t_req);

  } else {

    ControllerUtils::set_request_reply(Reply::r_301, t_req);
  }
}

void UserControllers::add(std::string &t_contact_username, Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");
  User contact = m_pg.search_user_by(t_contact_username, "username");

  if(user.username() == contact.username()){ // user cannot add itself.
    ControllerUtils::set_request_reply(Reply::r_300, t_req);
     return;
  }

  if(m_pg.user_contact_exists(user, contact)){
    ControllerUtils::set_request_reply(Reply::r_302, t_req);
     return;
  }

  if (!user.empty() && !contact.empty()) {

    bool res = m_pg.add_user_contact(user, contact);
    ControllerUtils::set_request_reply(res, t_req);

  } else {
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
  }
}

void UserControllers::remove(std::string &t_contact_username, Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");
  User contact = m_pg.search_user_by(t_contact_username, "username");

  if (!user.empty() && !contact.empty()) {

    bool res = m_pg.remove_user_contact(user, contact);
    ControllerUtils::set_request_reply(res, t_req);

  } else {
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
  }
}

void UserControllers::ping(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if (user.empty()) {
    ControllerUtils::set_request_reply(Reply::r_301,
                      t_req); // didn't find a user. return not found

  } else if (!user.online()) {
    ControllerUtils::set_request_reply(Reply::r_202,
                      t_req); // user is not online. Ask to login again.

  } else {
    ControllerUtils::set_request_reply(Reply::r_200, t_req); // user is online. return OK
  }
}

void UserControllers::available(std::string &t_username, Request &t_req) {

  User user = m_pg.search_user_by(t_username, "username");

  if (user.empty()) {
    ControllerUtils::set_request_reply(Reply::r_301, t_req); // not found
  }

  else if (!user.online()) {
    ControllerUtils::set_request_reply(Reply::r_300, t_req); // not online
  }

  else {
    ControllerUtils::set_request_reply(Reply::r_201, user.to_string(), t_req);
  }
}

void UserControllers::exit(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by(t_req.m_address, "address");

  // logoff sets address column to NULL online to FALSE.
  if (!user.empty()) { // if user is logged in then logoff.
    m_pg.logoff(user);
  }

  t_req.m_exit = true;
  ControllerUtils::set_request_reply(Reply::r_201, "Goodbye", t_req);
}

void UserControllers::none(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  ControllerUtils::set_request_reply(Reply::r_501, t_req); // invalid command
}

bool UserControllers::ip_exists(Request &t_req) {

  User user = m_pg.search_user_by(t_req.m_address, "address");

  if (user.empty()) {
    ControllerUtils::set_request_reply(Reply::r_202, t_req);
  }

  return !user.empty();
}

/**************** PRIVATE ****************/

void UserControllers::login_user(User &t_user, Request &t_req) {

  t_user.update("true", User::Online);
  t_user.update(t_req.m_address, User::Address);

  bool res = m_pg.update_user(t_user);

  ControllerUtils::set_request_reply(res, t_req);
}

void UserControllers::list_contacts(Users &t_contacts, Request &t_req) {

  if (has_contacts(t_contacts)) {

    std::string reply_msg;

    for (auto &contact : t_contacts) {
      reply_msg += contact.to_string() + CONTACT_DELIM;
    }

    reply_msg.pop_back(); // remove last delim
    ControllerUtils::set_request_reply(Reply::r_201, std::move(reply_msg), t_req);
  } else {

    ControllerUtils::set_request_reply(Reply::r_301, t_req);
  }
}

bool UserControllers::is_empty(std::string &t_user, std::string &t_password,
                           Request &t_req) {

  bool res = t_user.empty() || t_password.empty();

  if (res) {
    ControllerUtils::set_request_reply(Reply::r_501, t_req);
  }

  return res;
}

bool UserControllers::has_contacts(Users &t_contacts) {
  return 0 < t_contacts.size();
}
