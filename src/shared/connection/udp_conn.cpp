#include "udp_conn.hpp"
#include "doctest.h"
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// For tests only
#include "request.hpp"
#include "text_data.hpp"
#include "udp_text_io.hpp"

// initiate conn as UDP/DGRAM SOCKET.
UDPConn::UDPConn(int t_port, IOStrategy *t_io)
    : Connection(t_port, SOCK_DGRAM), m_io(t_io) {}

// unbound sockets does not need a port number so we don't need one. Kernel will
// assign upon sendfrom(4)
UDPConn::UDPConn(IOStrategy *t_io) : Connection(7000, SOCK_DGRAM), m_io(t_io) {}

auto UDPConn::bind_socket(const std::string &t_address) -> bool {

  // setup with rcvfrom timeout
  if (!setup(t_address, m_RECEIVE_TIMEOUT)) {
    return is_valid(-1, "Could not setup UDP connection.");
  }

  auto address = get_address();

  int result = bind(get_socket(), reinterpret_cast<struct sockaddr *>(&address),
                    sizeof(address));

  return is_valid(result, "Warning: Did not bind UDP socket. If you are a "
                          "client ignore this warning.");
}

void UDPConn::set_strategy(IOStrategy *t_io) {
  delete m_io;
  m_io = t_io;
};

auto UDPConn::receive(Request &t_req) -> bool {
  t_req.m_socket = get_socket();

  if (t_req.m_valid) {
    t_req.m_valid = m_io->receive(t_req);
  }

  return t_req.m_valid;
}

auto UDPConn::respond(Request &t_req) -> bool {
  t_req.m_socket = get_socket();

  if (t_req.m_valid) {
    t_req.m_valid = m_io->respond(t_req);
  }

  return t_req.m_valid;
}
