#ifndef POSTGRES_H
#define POSTGRES_H
#include "logger.hpp"
#include <pqxx/pqxx>
#include <vector>
#include "user.hpp"

typedef std::vector<User> Users;

class Postgres {

public:
  Postgres();

  Users list_users();
  Users list_user_contacts(const User &t_user);

  User search(const std::string &t_username);
  User search(const char *t_username);
  bool add(const User &t_user);
  bool remove(const User &t_user);
  bool update(const User &t_user);

private:
  pqxx::connection m_conn;
  std::string m_table;

  std::string update_query(const User &t_user, pqxx::work &t_transaction) const;
  std::string list_contacts_query(const User &t_user, pqxx::work &t_transaction) const;
};
#endif // !POSTGRES_H
