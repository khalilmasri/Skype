#ifndef CALL_CONTROLLERS_H
#define CALL_CONTROLLERS_H

#include <tuple>

#include "request.hpp"
#include "postgres.hpp"
#include "user_awaiting.hpp"

struct CallControllers {
  static void  connect    (std::string &t_arg, Request &t_req);
  static void  accept     (std::string &t_arg, Request &t_req);
  static void  hangup     (std::string &_, Request &t_req);
  static void  reject     (std::string &t_arg, Request &t_req);
  static void  ping       (std::string &_, Request &t_req);

  /* call_awaits is a compromise that allows other controllers to access data from AwaitingUsers.
   *
   * When the client calls LIST the server will also return if any contact is attempting to
   * the user (that called LIST). For that it requires access to AwaitingUsers in this controller.
   * 
   * call_awaits does not modify AwaitingUsers
   * */

  static auto call_awaits (int t_user_id) -> bool;

  private:
  using Valid = std::tuple<bool, int, std::string>;

  static Postgres m_pg;
  static AwaitingUsers m_awaiting_users;

  static auto validate_user_and_argument(std::string &t_arg, Request &t_req, User &t_user) -> Valid;
  static auto validate_local_address(std::string &t_local_address, Request &t_req) -> bool;
};


#endif
