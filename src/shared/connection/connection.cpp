#include "connection.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

/* Public */

bool Connection::setup(const std::string &t_address) {
  m_is_setup = create_socket() && set_options() && set_address(t_address);
  return m_is_setup;
}

bool Connection::setup() {
  return create_socket();
}


bool Connection::reset_socket() {
  close(m_socket);
  m_is_setup = create_socket();
  return m_is_setup;
}

bool Connection::is_valid(int t_result, const char *t_msg,
                          ValidationLog t_log) const {

  if (t_result < 0 && t_log == Error) {
    LOG_ERR(t_msg);
  }

  else if (t_result < 0 && t_log == Debug) {
    LOG_DEBUG(t_msg);
  }

  else if (t_result < 0 && t_log == Info) {
    LOG_INFO(t_msg);
  }

  else if (t_result < 0 && t_log == Critical) {
    LOG_CRIT(t_msg);
  }

  return t_result >= 0;
}

bool Connection::is_setup() const {
  if (!m_is_setup) {
    LOG_ERR("The connection has not been set up.")
  }

  return m_is_setup;
}

int Connection::get_socket() const { return m_socket; }
sockaddr_in Connection::get_address() const { return m_address; }
int Connection::get_port() const { return m_port; };

sockaddr_in Connection::to_sockaddr_in(const std::string &t_port, const std::string &t_address) {
/* Static Public */

  sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));

  addr_in.sin_addr.s_addr = inet_addr(t_address.c_str());

  try {
    int port = std::stoi(t_port);
    addr_in.sin_port = htons(port);

  } catch (...) {
    LOG_ERR("Invalid port integer %s", t_port.c_str());
    addr_in.sin_port = htons(0); /// if cannot convert set port to 0.
  }

  addr_in.sin_family = AF_INET;

  return addr_in;
}

/* Private */

bool Connection::create_socket() {
  m_socket = socket(AF_INET, m_protocol, 0);
  return is_valid(m_socket, "Could not create socket.");
}

bool Connection::set_options() {

  int opt = 1;
  int res_addr =
      setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  int res_port =
      setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  return is_valid(res_port, "Invalid option - reuse port.") &&
         is_valid(res_addr, "Invalid options - reuse address.");
}

bool Connection::set_address(const std::string &_address) {

  int res = inet_pton(AF_INET, _address.c_str(),
                      &m_address.sin_addr); /* converts IP string */

  m_address.sin_family = AF_INET;
  m_address.sin_port = htons(m_port);

  return is_valid(res, "Invalid IP address.");
};

std::string Connection::address_tostring(sockaddr_in t_address) {

  char buffer[INET_ADDRSTRLEN] = {0};
  const char *ip_address = inet_ntop(
      AF_INET, reinterpret_cast<struct sockaddr *>(&t_address.sin_addr), buffer,
      INET_ADDRSTRLEN);

  return std::string(ip_address);
}

/* TEST
 *
 *  note that this is a base Connection class.
 *  you can only set up the connection here.
 *  Connection are specialized by subclasses ActiveConn and PassiveConn
 */

TEST_CASE("Connection") {

  auto conn = Connection(4000);
  bool res = conn.setup("127.0.0.1");

  SUBCASE("Valid socket") {
    // if got 0 means we close stdin which is probably a bug.
    CHECK(conn.get_socket() > 0);
  }

  SUBCASE("Valid setup") { CHECK(res == true); }

  close(conn.get_socket()); // close to avoid leak on tests.
}
