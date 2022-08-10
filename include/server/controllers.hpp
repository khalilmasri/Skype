#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "reply.hpp"
#include "request.hpp"
#include "postgres.hpp"

struct Controllers {

  static void  list      (std::string &_, Request &t_req);
  static void  create    (std::string &t_arg, Request &t_req);
  static void  login     (std::string &t_arg, Request &t_req);
  static void  search    (std::string &t_username, Request &t_req);
  static void  add       (std::string &t_contact_username, Request &t_req);
  static void  remove    (std::string &t_contact_username, Request &t_req);
  static void  ping      (std::string &_, Request &t_req);
  static void  available (std::string &_, Request &t_req);
  static void  exit      (std::string &_, Request &t_req);

  // this is called when router fails to parse the command. Return 500.
  static void  none      (std::string &_, Request &t_req); 

  // helper to check if user is logged in 
  static bool ip_exists(Request &t_req); 

  private:
  static Postgres m_pg;

   /* list */
  static void  list_contacts(Users &t_contacts, Request &t_req);

   /* login */
  static void  login_user(User &t_user, Request &t_req);
   

   /* helpers */
  static bool  is_empty(std::string &t_user, std::string &t_password, Request &t_req);
  static bool  has_contacts(Users &t_contacts);
  static void  set_request_reply(Reply::Code t_reply, Request &t_req);
  static void  set_request_reply(Reply::Code t_reply, std::string &&t_msg, Request &t_req);
  static void  set_request_reply(bool t_valid, Request &t_req);
};

#endif // !CONTROLLERS_H
      
