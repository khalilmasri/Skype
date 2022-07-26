#include "text_io.hpp"
#include "logger.hpp"
// -- test only
#include "active_conn.hpp"
#include "passive_conn.hpp"
#include "doctest.h"
//
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define HEADER_LENGTH 10

/* Public */

bool TextIO::receive(int t_socket, std::string &t_data) const {

  const int msg_length = read_header(t_socket);

  bool valid_header    = is_valid(msg_length, "Could not receive message header.");
  bool valid_msg;

  if (valid_header) {

    char* msg = new char[msg_length];
    int res   = recv(t_socket, msg, msg_length, 0);
    valid_msg = is_valid(res, "could not receive message.");

    if (valid_msg) {
      t_data = msg;
    }

    delete[] msg;
  }

  return valid_header && valid_msg;
}

bool TextIO::respond(int t_socket, std::string &t_data) const {

  int res = send(t_socket, t_data.c_str(), t_data.size(), 0);

  return is_valid(res, "Could not repond.");
};

/* Private */

int TextIO::read_header(int t_socket) const {

  char header[HEADER_LENGTH] = {0};
  int res = recv(t_socket, header, HEADER_LENGTH, 0);

  if (res == HEADER_LENGTH) { // read size must be the header length
    return std::stoi(header);
  }

  return -1;
}

bool TextIO::is_valid(int t_result, const char *t_msg) const {

  if (t_result < 0) {
    LOG_ERR(t_msg);
  }

  return t_result >= 0;
}

/* TESTS */

TEST_CASE("Text IO"){

  ActiveConn aconn(4000, new TextIO());
  auto ip = std::string("127.0.0.1");

  PassiveConn pconn(4000, new TextIO);
  pconn.bind_and_listen("127.0.0.1");

  aconn.connect_socket(ip);
  pconn.accept_connection();

  std::string msg("0000000012hello,world!");

  aconn.respond(msg); // active initiates with msg
  
  std::string received;
  pconn.receive(received);

  CHECK(received == "hello,world!"); // received by passive
  
  msg = "0000000016hello,indeed :)";
  pconn.respond(msg); // can only respond after it receives a msg from active.
  
  received = "";
  aconn.receive(received);

  CHECK(received == "hello,indeed :)"); // received by active
};



