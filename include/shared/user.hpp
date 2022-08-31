#ifndef USER_H
#define USER_H
#include <string>
#include <vector>

class User {
  public:
  enum Field {Username, Password, Online, Address, Port};
  typedef std::vector<Field> UpdatedFields;

  User();

  User(int t_id, std::string &t_username, std::string &t_password, bool t_online,
       std::string &t_address, std::string &t_port);

 User(int t_id, const char* t_username, const char* t_password, bool t_online,
     const char* t_address, std::string &t_port);

 User(int t_id, std::string &t_username, bool t_online, // constructor no pw
       std::string &t_address, std::string &t_port);


  bool        empty() const;
  std::string to_string() const;
  int         id() const;
  std::string username() const;
  std::string password() const;
  void        remove_password();
  bool        online() const;
  std::string address() const;
  std::string port() const;
  bool        update(std::string &t_value, Field t_field);
  bool        update(const char *t_value, Field t_field);

  UpdatedFields updated_fields() const;

  private:
  int           m_id;
  std::string   m_username;
  std::string   m_password;
  bool          m_online;
  std::string   m_address;
  std::string   m_port;
  bool          m_empty;
  UpdatedFields m_updated_fields;

  bool       set_online(std::string &t_value);
};
#endif

