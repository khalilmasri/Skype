#include "connection.hpp"
#include "logger.hpp"

/* C headers */
#include <arpa/inet.h>
#include <netinet/in.h> /* sockeaddr_in */
#include <sys/socket.h>
#include <unistd.h>

Connection::Connection(int _port) : m_port(_port){};

bool Connection::setup(const std::string &_address) {
  return create_socket() && set_options() && set_address(_address);
}

int Connection::get_socket() const { return m_socket; }

/* PRIVATE */

bool Connection::create_socket() {
  // bzero(&m_address, sizeof(m_address));
  m_socket = socket(AF_INET, SOCK_STREAM, 0);

  return is_valid(m_socket, "Could not create socket.");

  return true;
}

bool Connection::set_options() {

  int opt = 1;
  int res = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                       sizeof(opt));

  return is_valid(res, "Invalid option.");
}

bool Connection::set_address(const std::string &_address) {

  int res = inet_pton(AF_INET, _address.c_str(), &m_address.sin_addr); /* converts IP string */

  m_address.sin_family = AF_INET;
  m_address.sin_port = htons(m_port);

  return is_valid(res, "Invalid IP address.");
};

bool Connection::is_valid(int _result, const char *_msg) const {

  if (_result < 0) {
    LOG_ERR(_msg);
  }

  return _result >= 0;
}
