#include "passive_conn.hpp"
#include "IO_strategy.hpp"
#include "logger.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define QUEUE_SIZE 10

/* Constructor & Destructor */
PassiveConn::PassiveConn(int t_port, IOStrategy* t_io): Connection(t_port), m_io(t_io){};

PassiveConn::~PassiveConn() {
    shutdown(get_socket(), SHUT_RDWR);
    m_poll.close_all(); // close all connected socket fds in poll;
    delete m_io;
  }

/* Public */

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

// TODO: TAKE REQ FOR ACCEPT AS WELL.

Request PassiveConn::accept_connection() {

  Request req;
  // accepts only of there is an awaiting client trying to connect connection must also be set up.
  if (!is_setup() || !m_poll.accept_awaits()) {
    return req;
  }

  sockaddr_in address;
  socklen_t addr_len = sizeof(address);
  req.m_socket = accept(get_socket(), reinterpret_cast<struct sockaddr *>(&address),
                   &addr_len);

  req.m_valid = is_valid(req.m_socket, "Could not accept connection.", Connection::Debug);

  if (req.m_valid) {
    m_poll.add_socket(req.m_socket); // add new connections to poll.
    m_addresses[req.m_socket] = Connection::address_tostring(address); // store copy of IP address.
    req.m_address = Connection::address_tostring(address);
  }

  return req;
}


bool PassiveConn::receive(Request &t_req) {
  t_req.m_socket = m_poll.select_socket_with_events();
  t_req.m_valid  = is_valid(t_req.m_socket, "Nothing to receive.", Connection::Debug);

  if (t_req.m_valid) { // only receive when pending msg exists.
    t_req.m_valid = m_io->receive(t_req);
    t_req.m_address = m_addresses.at(t_req.m_socket); // req has now the socket IP address.
  }

  return t_req.m_valid;
}

bool PassiveConn::respond(Request &t_req) {

  if (t_req.m_valid) {
    t_req.m_valid = m_io->respond(t_req);
  }

  return t_req.m_valid;
}


void PassiveConn::disconnect_client(Request &t_req){
   m_poll.remove_socket(t_req.m_socket);
   close(t_req.m_socket);
}


