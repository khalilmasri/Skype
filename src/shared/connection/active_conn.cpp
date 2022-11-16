#include "active_conn.hpp"
#include "IO_strategy.hpp"
// -- test only
#include "doctest.h"
#include "passive_conn.hpp"
// --
#include "text_io.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

ActiveConn::ActiveConn() : Connection(), m_io(nullptr) {}; 

ActiveConn::ActiveConn(int t_port, IOStrategy *t_io): Connection(t_port), m_io(t_io){};


ActiveConn::~ActiveConn() {
  if(m_io != nullptr){
    delete m_io;
  }

  int socket = get_socket();
  if(socket > 0)
    close(socket);
}

Request ActiveConn::connect_socket(std::string &t_address) {

  Request req;

  req.m_valid = setup(t_address);

  if (!req.m_valid) {
    return req;
  }

  sockaddr_in address = get_address();
  req.m_address       = Connection::address_tostring(address);
  req.m_socket        = get_socket();

  struct sockaddr *addr_ref = reinterpret_cast<struct sockaddr *>(&address);
  int res                   = connect(get_socket(), addr_ref, sizeof(address));
  req.m_valid               = is_valid(res, "Could not connect to socket");

  return req;
}

bool ActiveConn::receive(Request &t_req) {
  t_req.m_socket = get_socket(); // just to keep convention as ActiveConn
                                 // handles only a single socket.

  if (t_req.m_valid) {
    t_req.m_valid = m_io->receive(t_req);
  }

  return t_req.m_valid;
}

bool ActiveConn::respond(Request &t_req) {
   t_req.m_socket = get_socket();

  if (t_req.m_valid) {
    t_req.m_valid = m_io->respond(t_req);
  }

  return t_req.m_valid;
}

/* TEST */

TEST_CASE("Active Connection") {

  ActiveConn aconn(4000, new TextIO());
  auto ip = std::string("127.0.0.1");

  SUBCASE("PassiveConn fails connection to bad server.") {
    Request req = aconn.connect_socket(ip);
    CHECK(req.m_valid == false);
  }

  SUBCASE("PassiveConn connects successfully") {
    PassiveConn pconn(4000, new TextIO);
    pconn.bind_and_listen("127.0.0.1");
    Request req = aconn.connect_socket(ip);
    pconn.accept_connection();

    CHECK(req.m_valid == true);
    CHECK(req.m_address == "127.0.0.1");
  }
};
