#include "text_io.hpp"
#include "logger.hpp"
// -- test only
#include "active_conn.hpp"
#include "doctest.h"
#include "passive_conn.hpp"
#include "request.hpp"
#include "text_data.hpp"
//
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#define HEADER_LENGTH 10

/* Public */

bool TextIO::receive(Request &t_req) const {

  const int msg_length = read_header(t_req.m_socket);

  bool valid_header = is_valid(msg_length, "Could not receive message header.");
  bool valid_msg;

  if (valid_header) {

    char *received = new char[msg_length + 1]; // +1 for \0
    memset(received, 0, msg_length + 1);
    
    int res = recv(t_req.m_socket, received, msg_length, MSG_WAITALL);
    valid_msg = is_valid(res, "could not receive message.");

    if (valid_msg) {
      t_req.set_data(new TextData(received));
    }

    delete[] received;
  }

  return valid_header && valid_msg;
}

bool TextIO::respond(Request &t_req) const {

  int res = -1;

  if (!t_req.data_empty() && t_req.data_type() == Data::Text) {

    std::string msg = TextData::to_string(t_req.data());
    std::string header = create_header(msg.size());
    header.append(msg);

    res = send(t_req.m_socket, header.c_str(), header.size(), 0);
  }

  return is_valid(res, "Could not respond.");
};

/* Private */

int TextIO::read_header(int t_socket) const {
  char header[HEADER_LENGTH + 1] = {0}; // + 1 must add one for \0
  int res = recv(t_socket, header, HEADER_LENGTH, 0);
  std::string s(header);
  
  if (res == HEADER_LENGTH) { // read size must be the header length
    try{
    return std::stoi(header);
    } catch(...) {
      LOG_ERR("The header is not a number: %s", header);
      return 0;
    }
  }

  return -1;
}

std::string TextIO::create_header(int t_msg_length) const {

  std::string msg_length = std::to_string(t_msg_length);
  std::string padding(HEADER_LENGTH - msg_length.size(), '0');

  return padding + msg_length;
}


/* TESTS */

TEST_CASE("Text IO & Connection") {

  ActiveConn aconn(4000, new TextIO());
  auto ip = std::string("127.0.0.1");
  PassiveConn pconn(4000, new TextIO);
  pconn.bind_and_listen("127.0.0.1");

  aconn.connect_socket(ip);

  Request req = pconn.accept_connection();

  std::string msg("hello,world!");

  req.set_data(new TextData(msg));
  aconn.respond(req); // active initiates with msg

  pconn.receive(req);

  SUBCASE("Passive Receives from valid IP adddress.") {
    CHECK(req.m_address == "127.0.0.1");
  }

  SUBCASE("Passive receives valid message.") {
    CHECK(TextData::to_string(req.data()) == msg); // received by passive
    CHECK(req.m_valid == true);
  }

  msg = "hello,indeed";
  req.set_data(new TextData(msg));
  pconn.respond(req); // can only respond after it receives a msg from active.

  aconn.receive(req);

  SUBCASE("Active receives valid message.") {
    CHECK(TextData::to_string(req.data()) == msg); // received by active
  }
};
