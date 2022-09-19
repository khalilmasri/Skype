#ifndef USER_CONTROLLERS_H
#define USER_CONTROLLERS_H

#include "reply.hpp"
#include "request.hpp"
#include "postgres.hpp"

struct UserControllers {

  static void  list      (std::string &_, Request &t_req);
  static void  create    (std::string &t_arg, Request &t_req);
  static void  login     (std::string &t_arg, Request &t_req);
  static void  search    (std::string &t_username, Request &t_req);
  static void  add       (std::string &t_contact_username, Request &t_req);
  static void  remove    (std::string &t_contact_username, Request &t_req);
  static void  available (std::string &_, Request &t_req);
  static void  exit      (std::string &_, Request &t_req);

  // this is called when router fails to parse the command. Return 500.
  static void  none      (std::string &_, Request &t_req); 

  // helper to check if user is logged in 

  static bool ip_exists(Request &t_req); 
  // checks token is valid
  static bool is_valid_token(std::string &t_token, Request &t_req);

  private:
  static Postgres m_pg;
  static const std::size_t  m_TOKEN_LENGTH;
  static const std::string  m_CONTACT_DELIM;

   /* list */
  static void  list_contacts(Users &t_contacts, Request &t_req);

   /* login */
  static void  login_user(User &t_user, Request &t_req);
   

   /* helpers */
  static bool  is_empty(std::string &t_user, std::string &t_password, Request &t_req);
  static bool  has_contacts(Users &t_contacts);
};

#endif // !CONTROLLERS_H
      
