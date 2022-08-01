#ifndef REQUEST_H
#define REQUEST_H

#include "data.hpp"
#include "reply.hpp"
#include "data.hpp"
#include <string>

struct Request {
  int         m_socket;
  std::string m_address;
  bool        m_valid;

  Request(bool t_valid = false);
  ~Request();

  void        set_data(Data *t_data) ;
  Data::type  data_type();
  bool        data_empty();
  const Data* data() const; // data returns const to prevent being modified anywhere else.

private:
  Data *m_data;

};

#endif // !REQUEST_H
