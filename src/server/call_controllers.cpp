#include "call_controllers.hpp"
#include "controller_utils.hpp"
#include "string_utils.hpp"
#include "supress_unused_params_warnings.hpp"

Postgres CallControllers::m_pg = Postgres();
AwaitingUsers CallControllers::m_awaiting_users = AwaitingUsers();

/* */

void CallControllers::connect(std::string &t_arg, Request &t_req) {

  User user = m_pg.search_user_by_token(t_req.m_token);

  auto [valid, contact_id, local_address] = validate_user_and_argument(t_arg, t_req, user);

  valid = valid && validate_local_address(local_address, t_req);

  if (!valid) {
    return;
  }

  valid = m_awaiting_users.insert(AwaitingUser(user.id(), contact_id, t_req.m_address, local_address));

  if (valid) {
    ControllerUtils::set_request_reply(Reply::r_200, t_req);

    // user is always awating to connect
  } else {
    ControllerUtils::set_request_reply(Reply::r_306, t_req);
  }
};

/* */

void CallControllers::accept(std::string &t_arg, Request &t_req) {

  User user = m_pg.search_user_by_token(t_req.m_token);

  auto [valid, contact_id, local_address] = validate_user_and_argument(t_arg, t_req, user);

  valid = valid && validate_local_address(local_address, t_req);

  if (valid) {
    try {
      AwaitingUser &await_user = m_awaiting_users.get(contact_id);
      await_user.set_peer_address(t_req.m_address);
      await_user.set_peer_local_address(local_address);

      ControllerUtils::set_request_reply(Reply::r_201, await_user.address(), t_req);

    } catch (...) {
      LOG_ERR("AwaitingUser id %s does not exist.", t_arg.c_str());
      ControllerUtils::set_request_reply(Reply::r_307, t_req);
    }
  }
};

/* */

void CallControllers::hangup(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by_token(t_req.m_token);

  if (user.empty()) { // user doesn't exists
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
    return;
  }

  bool valid = m_awaiting_users.destroy(user.id());

  if (valid) {
    ControllerUtils::set_request_reply(Reply::r_200, t_req);
  } else {
    ControllerUtils::set_request_reply(Reply::r_307, t_req);

  }
};


/* */

void  CallControllers::reject(std::string &t_arg, Request &t_req){

  // when the accepting peer rejects it removes the awaiting user from the AwaitingUserMap.
  User user = m_pg.search_user_by_token(t_req.m_token);
  auto [valid, contact_id, _] = validate_user_and_argument(t_arg, t_req, user);

  if(!valid){
     return;
  }

  valid = m_awaiting_users.destroy(contact_id);

  if (valid) {
    ControllerUtils::set_request_reply(Reply::r_200, t_req);
  } else {
    ControllerUtils::set_request_reply(Reply::r_307, t_req);
  }
}


/* */

void CallControllers::ping(std::string &_, Request &t_req) {
  UNUSED_PARAMS(_);

  User user = m_pg.search_user_by_token(t_req.m_token);

  if(user.empty()) {
     ControllerUtils::set_request_reply(Reply::r_301, t_req);
     return;
  }
   try {
      AwaitingUser await_user = m_awaiting_users.get(user.id());


    if(await_user.peer_address().empty()) {
      // peer_address is empty when peer has not yet accepted the call
       ControllerUtils::set_request_reply(Reply::r_203, t_req);

    } else {
       ControllerUtils::set_request_reply(Reply::r_201, await_user.peer_address(), t_req);

    }

   } catch(...) {
     ControllerUtils::set_request_reply(Reply::r_307, t_req);

   }
}

/* */

bool CallControllers::call_awaits(int t_user_id) {
 return m_awaiting_users.exists(t_user_id);
}

/* Private */

CallControllers::Valid
CallControllers::validate_user_and_argument(std::string &t_arg, Request &t_req,
                                            User &t_user) {

  if (t_user.empty()) { // user doesn't exists
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
    return {false, -1, {}};
  }

  auto[id_str, private_address] = StringUtils::split_first(t_arg);

  auto [valid, contact_id] = StringUtils::to_int(id_str);

  if (!valid) { // invalid argument
    ControllerUtils::set_request_reply(Reply::r_500, t_req);
    return {false, -1, {}};
  }

  return {true, contact_id, private_address};
}

bool CallControllers::validate_local_address(std::string &t_local_address, Request &t_req){

  if(t_local_address.empty()){
    ControllerUtils::set_request_reply(Reply::r_501, t_req);
    return false;
  }

  return true;
}
