#ifndef POSTGRES_H
#define POSTGRES_H
#include "logger.hpp"
#include <pqxx/pqxx>
#include <vector>
#include "user.hpp"

typedef std::vector<User> Users;
typedef std::tuple<std::string, std::string, std::string, std::string, std::string> AggregatedQueryResult;

class Postgres {

public:
  Postgres();

  Users       list_users();
  Users       list_user_contacts(const User &t_user);
  User        search_user_by(const std::string &t_term, const char *t_field);
  User        search_user_by(const char *t_term, const char *t_field);
  User        search_user_contact(const User &t_user, const std::string &t_contact_username);
  User        search_user_contact(const User &t_user, const char *t_contact_username);
  bool        user_contact_exists(const User &t_user, const User &t_contact);

  bool        add_user(const User &t_user);
  bool        add_user_contact(const User &t_user, const User &t_contact);

  bool        remove_user(const User &t_user);
  bool        remove_user_contact(const User &t_user, const User &t_contact);
  bool        update(const User &t_user);

  bool        logoff(const User &t_user);

private:
  pqxx::connection m_conn;
  std::string      m_table;

  Users       result_to_users(AggregatedQueryResult &&t_results);
  std::string update_query(const User &t_user, pqxx::work &t_transaction) const;
  std::string list_contacts_query(const User &t_user, pqxx::work &t_transaction) const;
  std::string search_contact_query(const User &t_user, const std::string &t_contact_username, pqxx::work &t_transaction) const;
};
#endif // !POSTGRES_H
