#ifndef POSTGRES_H
#define POSTGRES_H
#include "logger.hpp"
#include <pqxx/pqxx>
#include <vector>
#include "config.hpp"
#include "user_chat.hpp"

#define MAX_DB_PATH_NAME 100

typedef std::vector<User> Users;
typedef std::vector<UserChat> UserChats;
typedef std::tuple<std::string, std::string, std::string, std::string> AggregatedQueryResult;

class Postgres {

public:
  Postgres();

  Users       list_users();
  Users       list_user_contacts(const User &t_user);
  UserChats   list_user_chats(const User &t_user, const bool t_pending, const User &t_sender = User());

  User        search_user_by_token(const std::string &t_token);
  User        search_user_by(const std::string &t_term, const char *t_field);
  User        search_user_by(const char *t_term, const char *t_field);
  UserChat    search_user_chat_by(const std::string &t_term, const char *t_field);
  User        search_user_contact(const User &t_user, const std::string &t_contact_username);
  User        search_user_contact(const User &t_user, const char *t_contact_username);
  bool        user_contact_exists(const User &t_user, const User &t_contact);

  bool        add_user_chat(const UserChat &t_chat);
  bool        add_user(const User &t_user);
  bool        add_user_contact(const User &t_user, const User &t_contact);
  bool        add_user_token(const User &t_user, const std::string &t_token);

  bool        remove_user(const User &t_user);
  bool        remove_user_contact(const User &t_user, const User &t_contact);
  bool        remove_user_token(const User &t_user);
  bool        update_user(const User &t_user);

  bool        set_user_chat_to_delivered(const UserChat &t_user_chat);

  bool        logoff(const User &t_user);

private:
  Config           *m_config;
  const std::string m_db;
  pqxx::connection  m_conn;
  std::string       m_table;

  Users       result_to_users(AggregatedQueryResult &&t_results);
  std::string update_user_query(const User &t_user, pqxx::work &t_transaction) const;
  std::string list_contacts_query(const User &t_user, pqxx::work &t_transaction) const;
  std::string search_contact_query(const User &t_user, const std::string &t_contact_username, pqxx::work &t_transaction) const;
};
#endif // !POSTGRES_H
