#include "connection.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

/* Public */

/* timeout defaults to 0 */
auto Connection::setup(const std::string &t_address, int timeout) -> bool {
  m_is_setup =
      create_socket() && set_options(timeout) && set_address(t_address);
  return m_is_setup;
}

auto Connection::setup() -> bool { return create_socket(); }

auto Connection::reset_socket() -> bool {
  close(m_socket);
  m_is_setup = create_socket();
  return m_is_setup;
}

auto Connection::is_valid(int t_result, const char *t_msg, ValidationLog t_log)
    -> bool {

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

  else if (t_result < 0 && t_log == Trace) {
    LOG_TRACE(t_msg);
  }

  return t_result >= 0;
}

auto Connection::is_setup() const -> bool {
  if (!m_is_setup) {
    LOG_ERR("The connection has not been set up.")
  }

  return m_is_setup;
}

auto Connection::get_socket() const -> int { return m_socket; }
auto Connection::get_address() const -> sockaddr_in { return m_address; }
auto Connection::get_port() const -> int { return m_port; };

/* Static Public */

auto Connection::address_tostring(sockaddr_in t_address) -> std::string {

  std::string buffer(INET_ADDRSTRLEN, '\0');

  const char *ip_address = inet_ntop(
      AF_INET, reinterpret_cast<struct sockaddr *>(&t_address.sin_addr),
      buffer.data(), INET_ADDRSTRLEN);

  return {ip_address};
}

auto Connection::port_tostring(sockaddr_in t_address) -> std::string {
  int port = htons(t_address.sin_port);
  return std::to_string(port);
}

auto Connection::to_sockaddr_in(const std::string &t_port,
                                const std::string &t_address) -> sockaddr_in {
  sockaddr_in addr_in;
  memset(&addr_in, 0, sizeof(addr_in));

  addr_in.sin_addr.s_addr = inet_addr(t_address.c_str());

  try {
    int port = std::stoi(t_port);
    addr_in.sin_port = htons(port);

  } catch (...) {
    LOG_ERR("Invalid port integer '%s'", t_port.c_str());
    addr_in.sin_port = htons(0); /// if cannot convert set port to 0.
  }

  addr_in.sin_family = AF_INET;

  return addr_in;
}

/* Private */

auto Connection::create_socket() -> bool {
  m_socket = socket(AF_INET, m_protocol, 0);
  return is_valid(m_socket, "Could not create socket.");
}

auto Connection::set_options(int timeout) const -> bool {

  int res_timeout = 0;
  int opt = 1;
  int res_addr =
      setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  int res_port =
      setsockopt(m_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  if (timeout > 0) { // only set timeout if greater than 0
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    res_timeout =
        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  }

  return is_valid(res_port, "Invalid option - reuse port.") &&
         is_valid(res_addr, "Invalid options - reuse address.") &&
         is_valid(res_timeout, "Invalid options - receive timeout.");
}

auto Connection::set_address(const std::string &_address) -> bool {

  int res = inet_pton(AF_INET, _address.c_str(),
                      &m_address.sin_addr); /* converts IP string */

  m_address.sin_family = AF_INET;
  m_address.sin_port = htons(m_port);

  return is_valid(res, "Invalid IP address.");
};

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
