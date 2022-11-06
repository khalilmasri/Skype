#ifndef REQUEST_H
#define REQUEST_H

#include "data.hpp"
#include "reply.hpp"
#include <string>

struct Request {
  int         m_socket;
  std::string m_address;
  std::string m_token;
  bool        m_valid;
  bool        m_exit;

  explicit Request(bool t_valid = false, bool t_exit = false);
  explicit Request(const std::string &&t_address, bool t_valid = false, bool t_exit = false);
  explicit Request(const std::string &t_address, bool t_valid = false, bool t_exit = false);

  ~Request();

  void               set_data(Data *t_data) ;
  void               set_token(std::string &t_token);
  void               set_token(std::string &&t_token);
  auto               data_type() -> Data::Type;
  [[nodiscard]] auto data_empty() const -> bool;
  // data returns const to prevent being modified anywhere else.
  [[nodiscard]] auto data() const -> const Data*; 

private:
  Data *m_data;

};

#endif // !REQUEST_H
