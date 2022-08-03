#include "controllers.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include "text_data.hpp"
#include <iostream>

Postgres Controllers::m_pg = Postgres();

void Controllers::create(std::string &m_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(m_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user(0, username, password, false, t_req.m_address);
  bool result = m_pg.add(user);

  if (result) {
    set_request_reply(Reply::r_200, t_req);

  } else {
    set_request_reply(Reply::r_300, t_req);
  }
};

void Controllers::login(std::string &m_arg, Request &t_req) {

  auto [username, password] = StringUtils::split_first(m_arg);

  if (is_empty(username, password, t_req)) {
    return;
  }

  User user = m_pg.search(username);

  if (!user.empty() && user.password() == password) {
    set_request_reply(Reply::r_200, t_req);
  } else {
    set_request_reply(Reply::r_300, t_req);
  }
}



void  Controllers::search(std::string &t_arg, Request &t_req){

  if(t_arg.empty()){
    set_request_reply(Reply::r_501, t_req);
    return;
  }


}


/** PRIVATE **/

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

// TEST_CASE("Controllers") {
//
//   Request req;
//   Controllers::create(req);
// }
