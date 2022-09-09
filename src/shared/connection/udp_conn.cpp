#include "udp_conn.hpp"
#include "doctest.h"
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>


// For tests only
#include "request.hpp"
#include "text_data.hpp"
#include "data_io.hpp"

// initiate conn as UDP/DGRAM SOCKET.
UDPConn::UDPConn(int t_port, IOStrategy *t_io)
    : Connection(t_port, SOCK_DGRAM), m_io(t_io) {} 
                                                    
// unbound sockets does not need a port number so we don't need one. Kernel will assign updon sendfrom(4)
 UDPConn::UDPConn(IOStrategy *t_io)
    : Connection(0, SOCK_DGRAM), m_io(t_io) {} 

bool UDPConn::bind_socket(const std::string &t_address) {

  bool valid = setup(t_address);

    if(!is_valid(valid, "Could not setup UDP connection.")){
     return valid;
   }

  auto address = get_address();

  valid = bind(get_socket(), reinterpret_cast<struct sockaddr *>(&address),
                 sizeof(address));

  return is_valid(valid, "Could not bind socket in UDP connection.");
}

bool UDPConn::receive(Request &t_req) {
  t_req.m_socket = get_socket();

  if (t_req.m_valid) {
    t_req.m_valid = m_io->receive(t_req);
  }

  return t_req.m_valid;
}

bool UDPConn::respond(Request &t_req) {
  t_req.m_socket = get_socket();

  if (t_req.m_valid) {
    t_req.m_valid = m_io->respond(t_req);

  }

  return t_req.m_valid;
}

TEST_CASE("UDPconn") {

//  auto conn = UDPConn(new DataIO());
//  conn.setup();
//
//  Request req;
// // "206.189.0.154:7000";
//
//  req.set_data(new TextData("hello world"));
//  req.m_address = "206.189.0.154:7000";
// // req.m_address = "127.0.0.1:7000";
//  req.m_valid = true;
//  conn.respond(req);
//
//  conn.receive(req);
//
//  std::cout << "receiving..... " << std::endl;
//  std::cout << TextData::to_string(req.data()) << std::endl;
}
