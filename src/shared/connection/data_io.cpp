#include "data_io.hpp"
#include "connection.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#include <errno.h>

#define BUFFER_SIZE 100

bool DataIO::respond(Request &t_req) const {

  auto[ip, port] = StringUtils::split_first(t_req.m_address,":");

  std::string data = TextData::to_string(t_req.data());

  sockaddr_in addr_in = Connection::to_sockaddr_in(port, ip);

 int res = sendto(t_req.m_socket, data.c_str(), data.size(), 0,
      reinterpret_cast<struct sockaddr *>(&addr_in), sizeof(addr_in));

  return is_valid(res, "DataIO could not send.");
};


bool DataIO::receive(Request &t_req) const {

  auto[ip, port] = StringUtils::split_first(t_req.m_address,":");

  char buffer[BUFFER_SIZE] = {0};

  sockaddr_in socket_addr;
  socklen_t addr_len = sizeof(socket_addr);

  int res = recvfrom(t_req.m_socket, buffer, BUFFER_SIZE, 0,
      reinterpret_cast<struct sockaddr *>(&socket_addr), &addr_len);

  bool valid =  is_valid(res, "DataIO could not receive.");

 if(valid){
  t_req.m_address = Connection::address_tostring(socket_addr);
  t_req.set_data(new TextData(buffer));
 }

  return valid; 
};


// TODO: MANAGED TO SEND. needs to test receive and set up the punch hole UDP
