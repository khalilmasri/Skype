#include "user.hpp"


User::User(int t_id, std::string &t_username, std::string &t_password,
           bool t_online, std::string &t_address)
    : m_id(t_id), m_username(t_username), m_password(t_password),
      m_online(t_online), m_address(t_address), m_empty(false) {}

User::User(int t_id, const char *t_username, const char *t_password,
           bool t_online, const char *t_address)
    : m_id(t_id), m_username(t_username), m_password(t_password),
      m_online(t_online), m_address(t_address), m_empty(false) {}

User::User(int t_id, std::string &t_username,
           bool t_online, std::string &t_address)
    : m_id(t_id), m_username(t_username), m_password(),
      m_online(t_online), m_address(t_address), m_empty(false) {}


User::User() : m_empty(true) {}

int User::id() const { return m_id; }

std::string User::username() const { return m_username; }

std::string User::password() const { return m_password; }

bool User::online() const { return m_online; }

std::string User::address() const { return m_address; }

User::UpdatedFields User::updated_fields() const { return m_updated_fields; }

bool User::update(std::string &t_value, User::Field t_field) {

  switch (t_field) {
  case Online:
    return set_online(t_value);

  case Username:
    m_username = t_value;
    m_updated_fields.push_back(Username);
    return true;

  case Password:
    m_password = t_value;
    m_updated_fields.push_back(Password);
    return true;

  case Address:
    m_address = t_value;
    m_updated_fields.push_back(Address);
    return true;
  }

}


std::string User::to_string() const {
  std::string id = "| id: ";
  std::string pw = " | password: ";
  std::string user = " | username: ";
  std::string online = " | online: ";
  std::string address = " | address:";
  std::string end = " |\n";

  return id + std::to_string(m_id) + user + m_username + pw + m_password +
         online + (m_online ? "true" : "false") + address + m_address + end;
}

bool User::empty() const { return m_empty; }

bool User::set_online(std::string &t_value) {
  if (t_value == "true" || t_value == "t") {
    m_online = true;
  } else if (t_value == "false" || t_value == "f") {
    m_online = true;
  } else {
    return false;
  }

  m_updated_fields.push_back(Online);
  return true;
}
