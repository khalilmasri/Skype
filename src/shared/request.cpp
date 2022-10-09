#include "request.hpp"
#include "doctest.h"

Request::Request(bool t_valid, bool t_exit)
    : m_socket(-1), m_valid(t_valid), m_exit(t_exit), m_data(nullptr){};

Request::Request(const std::string &&t_address, bool t_valid, bool t_exit)
    : m_socket(-1), m_address(t_address), m_valid(t_valid), m_exit(t_exit), m_data(nullptr){};

Request::Request(const std::string &t_address, bool t_valid, bool t_exit)
    : m_socket(-1), m_address(t_address), m_valid(t_valid), m_exit(t_exit), m_data(nullptr){};

Request::~Request() { delete m_data; }

void Request::set_data(Data *t_data) {
    delete m_data;
   m_data = t_data;
};

void Request::set_token(std::string &token) { m_token = token;}
void Request::set_token(std::string &&token) { m_token = token;}

Data::type Request::data_type() { return m_data->get_type(); }
bool Request::data_empty() { return m_data == nullptr; }
const Data *Request::data() const { return m_data; }
