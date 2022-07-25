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

bool ActiveConn::connect_socket(std::string &t_address) {

  bool setup_result = setup(t_address);

  if (!setup_result) {
    return setup_result;
  }

  auto address = get_address();

  auto *addr_ref = reinterpret_cast<struct sockaddr *>(&address);
  int res = connect(get_socket(), addr_ref, sizeof(address));

  return is_valid(res, "Could not connect to socket");
}

bool ActiveConn::receive(std::string &t_data) { return m_io->receive(get_socket(), t_data); }
bool ActiveConn::respond(std::string &t_data) { return m_io->respond(get_socket(), t_data); }

/* TEST
 */

TEST_CASE("Active Connection"){
    
  ActiveConn aconn(4000, new TextIO());
  auto ip = std::string("127.0.0.1");

  SUBCASE("PassiveConn fails connection to bad server."){
     int res = aconn.connect_socket(ip);
     CHECK(res == false);
  }

  SUBCASE("PassiveConn connects successfully"){
     PassiveConn pconn(4000, new TextIO);
     pconn.bind_and_listen("127.0.0.1");
     int res = aconn.connect_socket(ip);
     pconn.accept_connection();
     
     CHECK(res == true);
  }
};
