#include "postgres.hpp"
#include "config.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "string_utils.hpp"
#include "supress_unused_params_warnings.hpp"
#include <exception>
#include <string.h>
#include <tuple>
#include <vector>

typedef std::vector<std::string> StringVector;
typedef const std::string ConstStr;

Postgres::Postgres()
    : m_config(Config::get_instance()), m_db(m_config->get_db()),
      m_conn(m_db){
          LOG_INFO("Connected to database: %s", m_conn.dbname());
      };

Users Postgres::list_users() {
  Users users;

  try {
    pqxx::work transaction(m_conn);
    pqxx::result res = transaction.exec("SELECT * FROM users;");
    transaction.commit();

    res.for_each([&users](int id, std::string username, std::string _,
                          bool online, std::string address) {
      UNUSED_PARAMS(_);
      users.push_back(User(id, username, online, address));
    });

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
  }

  return users;
}

/* */

UserChats Postgres::list_user_chats(const User &t_user, const bool t_pending,
                                    const User &t_sender) {
  UserChats user_chats;

  try {
    pqxx::work transaction(m_conn);

    std::string query = "SELECT DISTINCT id, created_at, sender_id, "
                        "recipient_id, text, delivered"
                        " FROM chats WHERE";

    query += " ( recipient_id = " + std::to_string(t_user.id());

    if (!t_sender.empty()) {
      query += " AND sender_id = " + std::to_string(t_sender.id());
    }

    if (t_pending) {
      query += " AND delivered = FALSE";
    }

    query += " ) ";

    // when command is a CHAT and no sender_id is provided return all chats for
    // the current user + it's own chat messages.
    if (!t_pending && t_sender.empty()) {
      query += " OR sender_id = " + std::to_string(t_user.id());
    }

    query += " ORDER BY created_at ASC;";

    pqxx::result res = transaction.exec(query);
    transaction.commit();

    res.for_each([&user_chats](int id, std::string created_at, int sender_id,
                               int recipient_id, std::string text,
                               bool delivered) {
      user_chats.push_back(
          UserChat(id, created_at, sender_id, recipient_id, text, delivered));
    });

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
  }

  return user_chats;
}

/* */

Users Postgres::list_user_contacts(const User &t_user) {

  AggregatedQueryResult result;

  try {
    pqxx::work transaction(m_conn);

    std::string query = list_contacts_query(t_user, transaction);
    pqxx::row row = transaction.exec1(query);

    transaction.commit();
    row.to(result);

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return Users(); // returns empty User vector
  }

  return result_to_users(std::move(result));
}

/* */

User Postgres::search_user_by_token(const std::string &t_token) {

  std::tuple<int, std::string, std::string, bool, std::string> results;

  try {
    pqxx::work transaction(m_conn);

    std::string query = "SELECT T.user_id, U.username, U.password, U.online, "
                        "COALESCE(U.address, ' ')"
                        " FROM tokens AS T"
                        " JOIN users AS U"
                        " ON T.user_id = U.id"
                        " WHERE t.token = " +
                        transaction.quote(t_token);

    pqxx::row row = transaction.exec1(query);

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

/* */

User Postgres::search_user_by(const char *t_term, const char *t_field) {
  std::string term(t_term);
  return search_user_by(term, t_field);
}

/* */

User Postgres::search_user_by(const std::string &t_term, const char *t_field) {

  std::tuple<int, std::string, std::string, bool, std::string> results;
  std::string field(t_field);

  try {
    pqxx::work transaction(m_conn);
    // COALESCE substitutes NULL with empty string
    pqxx::row row =
        transaction.exec1("SELECT id, username, password, online, "
                          "COALESCE(address, ' ') FROM users WHERE " +
                          field + " = " + transaction.quote(t_term));

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

/* */

User Postgres::search_user_contact(const User &t_user,
                                   const char *t_contact_username) {
  std::string contact(t_contact_username);
  return search_user_contact(t_user, contact);
}

/* */

User Postgres::search_user_contact(const User &t_user,
                                   const std::string &t_contact_username) {

  std::tuple<int, std::string, std::string, bool, std::string> results;

  try {
    pqxx::work transaction(m_conn);
    std::string query =
        search_contact_query(t_user, t_contact_username, transaction);

    pqxx::row row = transaction.exec1(query);

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

UserChat Postgres::search_user_chat_by(const std::string &t_term,
                                       const char *t_field) {

  std::tuple<int, std::string, int, int, bool, std::string> results;

  try {
    pqxx::work transaction(m_conn);

    std::string query = "SELECT * FROM chats WHERE " + std::string(t_field);
    query += " = " + t_term;

    pqxx::row row = transaction.exec1(query);

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return UserChat(); // return empty user
  }

  auto [id, created_at, sender, recipient, delivered, text] = results;

  return UserChat(id, created_at, sender, recipient, text, delivered);
}

bool Postgres::user_contact_exists(const User &t_user, const User &t_contact) {
  pqxx::row row;
  try {
    pqxx::work transaction(m_conn);
    std::string query = "SELECT id FROM contacts WHERE user_id = " +
                        transaction.quote(t_user.id()) + "AND contact_id = " +
                        transaction.quote(t_contact.id()) + ";";

    row = transaction.exec1(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return row.size() == 0 ? false : true;
}

/* */

bool Postgres::add_user(const User &t_user) {

  if (t_user.empty()) {
    LOG_DEBUG("Cannot add an empty user.");
    return false;
  }

  try {
    pqxx::work transaction(m_conn);

    std::string query =
        "INSERT INTO users (username, password, online, address) VALUES (";
    query += transaction.quote(t_user.username()) + ",";
    query += transaction.quote(t_user.password()) + ",";
    query +=
        (t_user.online() ? std::string("TRUE") : std::string("FALSE")) + ",";
    query += transaction.quote(t_user.address()) + ");";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::add_user_chat(const UserChat &t_chat) {

  if (t_chat.empty()) {
    LOG_DEBUG("Cannot add a new chat as an empty user.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);

    std::string query =
        "INSERT INTO chats (sender_id, recipient_id, text, delivered) VALUES (";
    query += transaction.quote(t_chat.sender()) + ",";
    query += transaction.quote(t_chat.recipient()) + ",";
    query += transaction.quote(t_chat.text()) + ",";
    query += (t_chat.delivered() ? std::string("TRUE") : std::string("FALSE")) +
             ");";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
};

/* */

bool Postgres::add_user_contact(const User &t_user, const User &t_contact) {

  if (t_user.empty() || t_contact.empty()) {
    LOG_DEBUG("Cannot add an empty user.");
    return false;
  }

  try {

    pqxx::work transaction(m_conn);
    std::string query = "INSERT INTO contacts (user_id, contact_id) VALUES (";
    query += transaction.quote(t_user.id()) + ",";
    query += transaction.quote(t_contact.id()) + ");";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::add_user_token(const User &t_user, const std::string &t_token) {

  if (t_user.empty()) {
    LOG_DEBUG("Cannot add token to an empty user.");
    return false;
  }

  try {
    pqxx::work transaction(m_conn);
    std::string query = "INSERT INTO tokens (user_id, token) VALUES (";
    query += transaction.quote(t_user.id()) + ", ";
    query += transaction.quote(t_token) + ");";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::remove_user(const User &t_user) {

  if (t_user.empty()) {
    LOG_DEBUG("Cannot remove an empty user.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);
    std::string query =
        "DELETE FROM users where id = " + transaction.quote(t_user.id());
    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::remove_user_token(const User &t_user) {

  if (t_user.empty()) {
    LOG_DEBUG("Cannot remove token from an empty user.");
    return false;
  }
  /* note that the app only support 1 session per user and will delete all
   * tokens related to a single user here
   * */
  try {
    pqxx::work transaction(m_conn);
    std::string query =
        "DELETE FROM tokens where user_id = " + transaction.quote(t_user.id());
    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::remove_user_contact(const User &t_user, const User &t_contact) {

  if (t_user.empty() || t_contact.empty()) {
    LOG_DEBUG("Cannot remove an empty user or contact.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);
    std::string query = "DELETE FROM contacts where user_id = " +
                        transaction.quote(t_user.id()) + " AND contact_id = " +
                        transaction.quote(t_contact.id()) + ";";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::update_user(const User &t_user) {
  if (t_user.empty()) {
    LOG_DEBUG("Cannot update an empty user.");

    return false;
  }

  try {
    pqxx::work transaction(m_conn);
    transaction.exec(update_user_query(t_user, transaction));
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

bool Postgres::set_user_chat_to_delivered(const UserChat &t_user_chat) {
  if (t_user_chat.empty()) {
    LOG_DEBUG("Cannot update an empty chat.");
    return false;
  }

  try {
    pqxx::work transaction(m_conn);
    std::string query = "UPDATE chats set delivered = TRUE WHERE ";
    query += "id = " + transaction.quote(t_user_chat.id()) + ";";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

bool Postgres::logoff(const User &t_user) {

  try {
    pqxx::work transaction(m_conn);
    std::string query =
        "UPDATE users SET address = NULL, online = FALSE WHERE id = " +
        transaction.quote(t_user.id()) + ";";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_DEBUG("%s", err.what());
    return false;
  }

  return true;
}

/**** PRIVATE ****/

Users Postgres::result_to_users(AggregatedQueryResult &&t_result) {
  auto [_, ids, contacts, onlines] = t_result;

  StringVector ids_vec = StringUtils::split(ids, ",");
  StringVector contacts_vec = StringUtils::split(contacts, ",");

  StringVector onlines_vec = StringUtils::split(onlines, ",");

  Users users;

  for (std::size_t i = 0; i < ids_vec.size();
       i++) { // vector will always have the same length
    bool online = onlines_vec.at(i) == "true" ? true : false;

    auto [_, id] =
        StringUtils::to_int(ids_vec.at(i)); // on error will return -1

    User user = User(id, contacts_vec.at(i), online);
    users.push_back(std::move(user));
  }

  return users;
}

/* */

std::string Postgres::update_user_query(const User &t_user,
                                        pqxx::work &t_transaction) const {

  std::string query = "UPDATE users SET ";

  for (auto &field : t_user.updated_fields()) {
    switch (field) {
    case User::Username:
      query += "username = " + t_transaction.quote(t_user.username()) + ",";
      continue;

    case User::Password:
      query += "password = " + t_transaction.quote(t_user.password()) + ",";
      continue;

    case User::Address:
      query += "address = " + t_transaction.quote(t_user.address()) + ",";
      continue;

    case User::Online:
      query += "online = " +
               t_transaction.quote(t_user.online() ? "TRUE" : "FALSE") + ",";
      continue;
    }
  }
  query.pop_back(); // remove trailing comnma
  query += " WHERE id = " + t_transaction.quote(t_user.id()) + ";";
  return query;
}

std::string Postgres::list_contacts_query(const User &t_user,
                                          pqxx::work &t_transaction) const {

  std::string query =
      "SELECT DISTINCT U.username, "
      "("
      "SELECT STRING_AGG(U1.id::VARCHAR, ',') AS id "
      "FROM contacts C1 "
      "INNER JOIN users U1 "
      "ON C1.contact_id = U1.id "
      "WHERE U.id = C1.user_id "
      "), "
      "( "
      "SELECT DISTINCT STRING_AGG(U2.username, ',') AS contacts "
      "FROM contacts C2 "
      "INNER JOIN users U2 "
      "ON C2.contact_id = U2.id "
      "WHERE U.id = C2.user_id "
      "), "
      "( "
      "SELECT DISTINCT STRING_AGG(U4.online::VARCHAR, ',') AS online "
      "FROM contacts C4 "
      "INNER JOIN users U4 "
      "ON C4.contact_id = U4.id "
      "WHERE U.id = C4.user_id "
      ") "
      "FROM contacts C "
      "JOIN users U "
      "ON C.user_id = U.id "
      "WHERE U.username = " +
      t_transaction.quote(t_user.username()) + ";";

  return query;
}

std::string
Postgres::search_contact_query(const User &t_user,
                               const std::string &t_contact_username,
                               pqxx::work &t_transaction) const {

  std::string query =
      "SELECT U.id, U.username, U.password, U.online, U.address "
      "FROM contacts C "
      "INNER JOIN users U "
      "ON C.contact_id = U.id "
      "WHERE C.user_id = " +
      t_transaction.quote(t_user.id()) +
      " AND U.username = " + t_transaction.quote(t_contact_username) + ";";

  return query;
}

/* !IMPORTANT
 *  You must have postgres setup to run these tests.
 *  1. install postgres
 *  2. create a database called `skype` then run `postgres_setup.sql` to create
 table and test rows.
 *       e.g. $ psql
 *
 */

TEST_CASE("Postgres (ensure that you have postgres setup)") {

  Postgres pg;

  SUBCASE("List users") {
    std::vector<std::string> list_cases{
        {"id:1,username:khalil,password:1234,online:false,address:123.453.3.1"},
        {"id:2,username:mario,password:1234,online:false,address:1.453.32.1"},
        {"id:3,username:shakira,password:1234,online:false,address:53.423.4.1"},
        {"id:4,username:dubius,password:1234,online:false,address:33.53.3.1"},
        {"id:5,username:martha,password:1234,online:true,address:127.0.0.1"}};

    Users users = pg.list_users();
    int i = 0;

    for (auto &user : users) {
      CHECK(user.to_string() == list_cases.at(i));
      i++;
    }
  }

  SUBCASE("list user contacts") {

    User user(0, "khalil", "123", true, "123");

    Users users = pg.list_user_contacts(user);
  }

  SUBCASE("Search users by username") {

    User u = pg.search_user_by("mario", "username");
    CHECK(u.empty() == false);
    CHECK(u.username() == "mario");
  }

  SUBCASE("Search user by IP address") {

    User u = pg.search_user_by("53.423.4.1", "address");
    CHECK(u.empty() == false);
    CHECK(u.username() == "shakira");
  }

  SUBCASE("Search for unexisting user") {

    User u = pg.search_user_by("jose", "username");
    CHECK(u.empty() == true);
  }

  SUBCASE("Search for a users contact") {

    User khalil = pg.search_user_by("khalil", "username");
    CHECK(khalil.empty() == false);

    std::string contact_username = "mario";
    User mario = pg.search_user_contact(khalil, contact_username);

    CHECK(mario.empty() == false);
    CHECK(mario.username() == contact_username);
  }

  SUBCASE("Add user contact") {

    User user = pg.search_user_by("khalil", "username");
    User contact = pg.search_user_by("dubius", "username");

    CHECK(user.empty() == false);
    CHECK(contact.empty() == false);

    pg.add_user_contact(user, contact);

    User dubius = pg.search_user_contact(user, contact.username());

    CHECK(dubius.username() == contact.username());
  }

  SUBCASE("Remove user contact") {

    User user = pg.search_user_by("khalil", "username");
    User contact = pg.search_user_contact(user, "dubius");

    CHECK(user.empty() == false);
    CHECK(contact.empty() == false);

    pg.remove_user_contact(user, contact);

    User dubius = pg.search_user_contact(user, contact.username());
    CHECK(dubius.empty());
  }

  SUBCASE("Add and Remove new user") {

    User u(0, "khalil", "123", true, "123");

    pg.add_user(u);

    User res = pg.search_user_by("pedro", "username");
    CHECK(res.username() == "pedro");

    pg.remove_user(res);

    res = pg.search_user_by("pedro", "username");
    CHECK(res.empty() == true);
  }

  SUBCASE("Update Existing user") {

    // add user to udate so we don't break other tests
    User u_add(0, "pedro", "1234", false, "123.43.453.12");
    pg.add_user(u_add);

    std::string username = "pedro";
    std::string to_update = "marco";

    User u = pg.search_user_by("pedro", "username"); // search for user we added
    bool res = u.update(to_update, User::Username);  // update object
    CHECK(res == true);
    pg.update_user(u); // update record

    // search for updated record
    User u2 = pg.search_user_by("marco", "username");
    CHECK(u2.empty() == false);
    CHECK(u2.username() == to_update);

    // update back to what it was
    res = u2.update(username, User::Username);
    CHECK(res == true);
    pg.update_user(u2);

    // check if it is like it was before
    User u3 = pg.search_user_by("pedro", "username");
    CHECK(u3.empty() == false);
    CHECK(u3.username() == username);

    pg.remove_user(u3); // remove user we added for test
  }
}
