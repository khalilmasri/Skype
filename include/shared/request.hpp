#ifndef REQUEST_H
#define REQUEST_H

#include "data.hpp"
#include "reply.hpp"
#include "data.hpp"
#include <string>

struct Request {
  int         m_socket;
  std::string m_address;
  std::string m_token;
  bool        m_valid;
  bool        m_exit;

  Request(bool t_valid = false, bool t_exit = false);
  ~Request();

  void        set_data(Data *t_data) ;
  void        set_token(std::string &t_token);
  void        set_token(std::string &&t_token);
  Data::type  data_type();
  bool        data_empty();
  const Data* data() const; // data returns const to prevent being modified anywhere else.

private:
  Data *m_data;

};

#endif // !REQUEST_H
