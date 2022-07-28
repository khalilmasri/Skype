#ifndef REQUEST_H
#define REQUEST_H

#include <string>

struct Request { // TODO: add more members as necessary to handle a request
  int         m_socket; 
  std::string m_address;
  std::string m_data;
  bool        m_valid;

  Request(bool t_valid = false) : m_valid(t_valid) {}
};

#endif // !REQUEST_H
