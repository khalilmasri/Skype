#include "call_controllers.hpp"
#include "controller_utils.hpp"
#include "string_utils.hpp"
#include "supress_unused_params_warnings.hpp"

// TODO: TEST all the controllers again. You just added rejected.
//       Move PING to this controllers.
//       Add another field to LIST to inform peer that there is a connection request.
//       NOTE: m_awaiting_users will have to be accessed from UserController. Make it Global?

Postgres CallControllers::m_pg = Postgres();
AwaitingUsers CallControllers::m_awaiting_users = AwaitingUsers();

/* */

void CallControllers::connect(std::string &t_arg, Request &t_req) {

  User user = m_pg.search_user_by_token(t_req.m_token);

  auto [valid, contact_id] = validate_user_and_argument(t_arg, t_req, user);

  if (!valid) {
    return;
  }

  valid = m_awaiting_users.insert(
      AwaitingUser(user.id(), contact_id, t_req.m_address));

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

  auto [valid, contact_id] = validate_user_and_argument(t_arg, t_req, user);


  if (valid) {
    try {
      AwaitingUser &await_user = m_awaiting_users.get(contact_id);
      await_user.set_peer_address(t_req.m_address);

      ControllerUtils::set_request_reply(Reply::r_201, await_user.address(), t_req);

    } catch (...) {
      LOG_ERR("AwaitingUser id %s does not exist.", t_arg.c_str());
      ControllerUtils::set_request_reply(Reply::r_301, t_req);
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
    ControllerUtils::set_request_reply(Reply::r_300, t_req);
  }
};


/* */

void  CallControllers::reject(std::string &t_arg, Request &t_req){

  // when the accepting peer rejects it removes the awaiting user from the AwaitingUserMap.
  User user = m_pg.search_user_by_token(t_req.m_token);
  auto [valid, contact_id] = validate_user_and_argument(t_arg, t_req, user);

  if(!valid){
     return;
  }

  valid = m_awaiting_users.destroy(contact_id);

  if (valid) {
    ControllerUtils::set_request_reply(Reply::r_200, t_req);
  } else {
    ControllerUtils::set_request_reply(Reply::r_301, t_req);
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
    return {false, -1};
  }

  auto [valid, contact_id] = StringUtils::to_int(t_arg);

  if (!valid) { // invalid argument
    ControllerUtils::set_request_reply(Reply::r_500, t_req);
    return {false, -1};
  }

  return {true, contact_id};
}
