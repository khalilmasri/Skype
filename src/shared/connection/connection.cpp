#include "connection.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

/* Public */

bool Connection::setup(const std::string &t_address) {
  m_is_setup = create_socket() && set_options() && set_address(t_address);

  return m_is_setup;
}

bool Connection::is_valid(int t_result, const char *t_msg) const {
  if (t_result < 0) {
    LOG_ERR(t_msg);
  }
  return t_result >= 0;
}

bool Connection::is_setup() const {
  if (!m_is_setup){
    LOG_ERR("The connection has not been set up.")
  }

  return m_is_setup;
}


int Connection::get_socket() const { return m_socket; }
sockaddr_in Connection::get_address() const { return m_address; }
int Connection::get_port() const {return m_port;};

/* Private */

bool Connection::create_socket() {
  // bzero(&m_address, sizeof(m_address));
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
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
