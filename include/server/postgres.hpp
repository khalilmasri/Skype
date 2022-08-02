#ifndef POSTGRES_H
#define POSTGRES_H
#include "logger.hpp"
#include <pqxx/pqxx>
#include <vector>
#include "user.hpp"

typedef std::vector<User> Users;

class Postgres {

public:
  Postgres(const char *t_table);

  Users list();
  User search(const std::string &t_username);
  User search(const char *t_username);
  void add(const User &t_user);
  void remove(const User &t_user);
  void update(const User &t_user);

private:
  pqxx::connection m_conn;
  std::string m_table;

  std::string update_query(const User &t_user, pqxx::work &t_transaction) const;
};
#endif // !POSTGRES_H
