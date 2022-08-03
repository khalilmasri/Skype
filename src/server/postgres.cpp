#include "postgres.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <exception>
#include <tuple>

#define DB "postgresql://localhost:5432/skype"

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

Users Postgres::list_user_contacts(const User &t_user) {
  Users users;

  try {
    pqxx::work transaction(m_conn);

    std::string query = list_contacts_query(t_user, transaction);
    pqxx::result res = transaction.exec(query);

    transaction.commit();

    res.for_each([&users](std::string user, int contact_id, std::string contact_name,
                           std::string contact_addr, bool online) {

      users.push_back(User(contact_id, contact_name, online, contact_addr));
    });

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
  }

  return users;
}


User Postgres::search(const char *t_username) {
  std::string user(t_username);
  return search(user);
}

User Postgres::search(const std::string &t_username) {

  std::tuple<int, std::string, std::string, bool, std::string> results;

  try {
    pqxx::work transaction(m_conn);
    pqxx::row row = transaction.exec1("SELECT * FROM users WHERE username = " +
                                      transaction.quote(t_username));

    transaction.commit();
    row.to(results);

  } catch (const std::exception &err) {
    LOG_ERR("%s", err.what());
    return User(); // return empty user
  }

  auto [id, username, password, online, address] = results;

  return User(id, username, password, online, address);
}

bool Postgres::add(const User &t_user) {

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

bool Postgres::remove(const User &t_user) {

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

std::string Postgres::update_query(const User &t_user, pqxx::work &t_transaction) const {

  std::string query = "UPDATE users SET ";

  for(auto &field : t_user.updated_fields()){
    switch(field){
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
        query += "online = " + t_transaction.quote(t_user.online() ? "TRUE" : "FALSE") + ",";
        continue;
    }

  }
  query.pop_back(); // remove trailing comnma
  query += " WHERE id = " +  t_transaction.quote(t_user.id()) + ";";
  return query;
}


std::string Postgres::list_contacts_query(const User &t_user, pqxx::work &t_transaction) const {

  std::string query = 
    "SELECT U.username,"
    "(SELECT SRZ.id "
     "FROM contacts CONT "
     "INNER JOIN users SRZ "
     "ON CONT.contact_id = SRZ.id "
     "WHERE CONT.contact_id = C.contact_id "
    "), "
    "("
     "SELECT DISTINCT STRING_AGG(USR.username, ',') AS contacts "
    "FROM contacts CONT "
    "INNER JOIN users USR "
    "ON CONT.contact_id = USR.id "
    " WHERE CONT.contact_id = C.contact_id "
    "), "
   "( "
    "SELECT DISTINCT STRING_AGG(USR.address, ',') AS address "
     "FROM contacts CONT "
     "INNER JOIN users USR "
     "ON CONT.contact_id = USR.id "
     "WHERE CONT.contact_id = C.contact_id "
    "), "
    "( "
     "SELECT SR.online "
     "FROM contacts CONT "
     "INNER JOIN users SR "
     "ON CONT.contact_id = SR.id "
     "WHERE CONT.contact_id = C.contact_id "
    ") "
"FROM contacts C "
"JOIN users U "
"ON C.user_id = U.id "
"WHERE U.username = " + t_transaction.quote(t_user.username()) + ";";

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

//SELECT C.username, C.online, c.address FROM users U JOIN users C ON C.id = U.contacts;

TEST_CASE("Postgres (unsure you have postgres setup)") {

  Postgres pg;

  SUBCASE("List users") {
    std::vector<std::string> list_cases{
        {"| id: 1 | username: khalil | password: 1234 | online: false | address:123.453.3.1 |\n"},
        {"| id: 2 | username: mario | password: 1234 | online: false | address:1.453.32.1 |\n"},
        {"| id: 3 | username: shakira | password: 1234 | online: false | address:53.423.4.1 |\n"},
        {"| id: 4 | username: dubius | password: 1234 | online: false | address:33.53.3.1 |\n"}
    };

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

    std::string username = "mario";
    User u = pg.search(username);
    CHECK(u.empty() == false);
    CHECK(u.username() == username);
  }

  SUBCASE("Search for unexisting user") {

    std::string username = "jose";
    User u = pg.search(username);
    CHECK(u.empty() == true);
  }

  SUBCASE("Add and Remove new user") {

    User u(0, "Pedro", "1234", false, "123.43.453.12");
    pg.add(u);

    std::string username = "Pedro";
    User res = pg.search(username);
    CHECK(res.username() == username);

    pg.remove(res);

    res = pg.search(username);
    CHECK(res.empty() == true);
  }

  SUBCASE("Update Existing user") {

    // add user to udate so we don't break other tests
    User u_add(0, "pedro", "1234", false, "123.43.453.12");
    pg.add(u_add);

    std::string username = "pedro";
    std::string to_update = "marco";

    User u = pg.search(username); // search for user we added
    bool res = u.update(to_update, User::Username); // update object
    CHECK(res == true);
    pg.update(u); // update record

    // search for updated record
    User u2 = pg.search(to_update);
    CHECK(u2.empty() == false);
    CHECK(u2.username() == to_update);

    // update back to what it was
    res = u2.update(username, User::Username);
    CHECK(res == true);
    pg.update(u2);

    // check if it is like it was before
    User u3 = pg.search(username);
    CHECK(u3.empty() == false);
    CHECK(u3.username() == username);

    pg.remove(u3); // remove user we added for test

  }

}
