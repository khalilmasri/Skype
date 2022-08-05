#include "postgres.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "string_utils.hpp"
#include <exception>
#include <tuple>
#include <vector>
#include "supress_unused_params_warnings.hpp"

#define DB "postgresql://localhost:5432/skype"

typedef std::vector<std::string > StringVector;

Postgres::Postgres() : m_conn(DB) {
  LOG_INFO("Connected to database: %s", m_conn.dbname());
};

Users Postgres::list_users() {
  Users users;

  try {
    pqxx::work transaction(m_conn);
    pqxx::result res = transaction.exec("SELECT * FROM users;");
    transaction.commit();

    res.for_each([&users](int id, std::string username, std::string password,
                          bool online, std::string address) {
      users.push_back(User(id, username, password, online, address));
    });

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
  }

  return users;
}

/* */

Users Postgres::list_user_contacts(const User &t_user) {

  AggregatedQueryResult result;

  try {
    pqxx::work transaction(m_conn);

    std::string query = list_contacts_query(t_user, transaction);
    pqxx::row row     = transaction.exec1(query);


    transaction.commit();
    row.to(result);

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return Users(); // returns empty User vector
  }

    return result_to_users(std::move(result));
}


User Postgres::search_user_by(const char *t_term, const char *t_field) {
  std::string term (t_term);
  return search_user_by(term, t_field );
}

/* */

User Postgres::search_user_by(const std::string &t_term, const char *t_field) {

  std::tuple<int, std::string, std::string, bool, std::string> results;
  std::string field(t_field);

  try {
    pqxx::work transaction(m_conn);
    pqxx::row row = transaction.exec1("SELECT * FROM users WHERE " + field + " = " +
                                      transaction.quote(t_term));

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

User Postgres::search_user_contact(const User &t_user, const char* t_contact_username) {
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
    LOG_ERR("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

/* */

bool Postgres::add_user(const User &t_user) {

  if (t_user.empty()) {
    LOG_ERR("Cannot add an empty user.");
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
    query += transaction.quote(t_user.address()) + " );";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::add_user_contact(const User &t_user, const User &t_contact) {

  if (t_user.empty() || t_contact.empty()) {
    LOG_ERR("Cannot add an empty user.");
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
    LOG_ERR("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::remove_user(const User &t_user) {

  if (t_user.empty()) {
    LOG_ERR("Cannot remove an empty user.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);
    std::string query =
        "DELETE FROM users where id = " + transaction.quote(t_user.id());
    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::remove_user_contact(const User &t_user, const User &t_contact) {

  if (t_user.empty() || t_contact.empty()) {
    LOG_ERR("Cannot remove an empty user or contact.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);
    std::string query =
        "DELETE FROM contacts where user_id = " + transaction.quote(t_user.id())
         + " AND contact_id = " + transaction.quote(t_contact.id()) + ";";

    transaction.exec(query);
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return false;
  }

  return true;
}

/* */

bool Postgres::update(const User &t_user) {

  if (t_user.empty()) {
    LOG_ERR("Cannot remove an empty user.");
    return false;
  }
  try {
    pqxx::work transaction(m_conn);
    transaction.exec(update_query(t_user, transaction));
    transaction.commit();

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return false;
  }

  return true;
}

/**** PRIVATE ****/

Users Postgres::result_to_users(AggregatedQueryResult &&t_result){
    auto [_, ids, contacts, onlines, addreses] = t_result;

    StringVector ids_vec       = StringUtils::split(ids, ",");
    StringVector contacts_vec  = StringUtils::split(contacts, ",");
    StringVector onlines_vec   = StringUtils::split(onlines, ",");
    StringVector addresses_vec = StringUtils::split(addreses, ",");

    Users users;

    for(std::size_t i = 0; i < ids_vec.size(); i++){ // vector will always have the same length
       bool online = onlines_vec.at(i) == "true" ? true : false;
       int id      = std::stoi(ids_vec.at(i));
       User user   = User(id, contacts_vec.at(i), online, addresses_vec.at(i));
       users.push_back(std::move(user));
    }

    return users;
}

/* */

std::string Postgres::update_query(const User &t_user,
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
     "), "
     "( "
       "SELECT DISTINCT STRING_AGG(U3.address, ',') AS address "
       "FROM contacts C3 "
       "INNER JOIN users U3 "
       "ON C3.contact_id = U3.id "
       "WHERE U.id = C3.user_id "
     ") "
    "FROM contacts C "
    "JOIN users U "
    "ON C.user_id = U.id "
    "WHERE U.username = "  + t_transaction.quote(t_user.username()) + ";";

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
        {"id:4,username:dubius,password:1234,online:false,address:33.53.3.1"}};

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

    for (auto &user : users) {

      std::cout << user.username() << "\n";
    }
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

    User u(0, "pedro", "1234", false, "123.43.453.12");
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

    User u = pg.search_user_by("pedro", "username");              // search for user we added
    bool res = u.update(to_update, User::Username); // update object
    CHECK(res == true);
    pg.update(u); // update record

    // search for updated record
    User u2 = pg.search_user_by("marco", "username");
    CHECK(u2.empty() == false);
    CHECK(u2.username() == to_update);

    // update back to what it was
    res = u2.update(username, User::Username);
    CHECK(res == true);
    pg.update(u2);

    // check if it is like it was before
    User u3 = pg.search_user_by("pedro", "username");
    CHECK(u3.empty() == false);
    CHECK(u3.username() == username);

    pg.remove_user(u3); // remove user we added for test
  }
}
