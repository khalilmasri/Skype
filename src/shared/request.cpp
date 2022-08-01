#include "request.hpp"
#include "doctest.h"

Request::Request(bool t_valid)
    : m_socket(-1), m_valid(t_valid), m_data(nullptr){};

Request::~Request() { delete m_data; }

void Request::set_data(Data *t_data) {
  if (m_data) {
    delete m_data;
  }

  m_data = t_data;
};

Data::type Request::data_type() { return m_data->get_type(); }
bool Request::data_empty() { return m_data == nullptr; }
const Data *Request::data() const { return m_data; }
