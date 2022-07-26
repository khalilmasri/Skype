#include "passive_conn.hpp"
#include "logger.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define QUEUE_SIZE 10

bool PassiveConn::bind_and_listen(const std::string &t_address) {

  setup(t_address);

  sockaddr_in address = get_address();

  int res = bind(get_socket(), reinterpret_cast<struct sockaddr *>(&address),
                 sizeof(address));

  bool valid = is_valid(res, "Could not bind to socket.");

  if (valid) {
    listen(get_socket(), QUEUE_SIZE);
    m_poll.add_socket(get_socket());
    LOG_INFO("Listening on %d.", get_port())
  }
  return valid;
}

bool PassiveConn::accept_connection() {

  // accepts only of there is an awaiting client trying to connect
  // connection must also be set up.
  if (!is_setup() || !m_poll.accept_awaits()) {
    return false;
  }

  sockaddr_in address = get_address();
  socklen_t addr_len = sizeof(address);
  int res = accept(get_socket(), reinterpret_cast<struct sockaddr *>(&address),
                   &addr_len);

  bool valid = is_valid(res, "Could not accept connection.", Connection::Debug);

    if (valid) {
      m_poll.add_socket(res); // add new connections to poll.
  }

  return valid;
}

bool PassiveConn::receive(std::string &t_data) {
  m_client_socket = m_poll.select_socket_with_events();

  bool valid = is_valid(m_client_socket, "Nothing to receive.", Connection::Debug);

  if (valid) { // only receive when pending msg exists.
    return m_io->receive(m_client_socket, t_data);
  } else {
    return valid;
  }
}

bool PassiveConn::respond(std::string &t_data) {

  // check if responding to an received client msg. ActiveConn must initiate msg exchange.
  bool valid = is_valid(m_client_socket, "No valid client socket to respond.", Connection::Debug);

  if (valid) {
    bool res = m_io->respond(m_client_socket, t_data);
    m_client_socket = -1; // reset
    return res;
  } else {
    return valid;
  }
}
