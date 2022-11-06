#include "udp_text_io.hpp"
#include "connection.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

#include <cstring>
#include <netinet/in.h>
#include <algorithm>
#include <arpa/inet.h>
#include <algorithm>
#include <unistd.h>
#include <cerrno>
#include <array>

auto UDPTextIO::respond(Request &t_req) const -> bool {

  auto[ip, port] = StringUtils::split_first(t_req.m_address,":");

  std::string buffer  = TextData::to_string(t_req.data());
  sockaddr_in addr_in = Connection::to_sockaddr_in(port, ip);

 ssize_t res = sendto(t_req.m_socket, buffer.data(), buffer.size(), 0,
      reinterpret_cast<struct sockaddr *>(&addr_in), sizeof(addr_in));

  t_req.m_valid = is_valid(static_cast<int>(res), "UDPTextIO could not send.");

  return t_req.m_valid;
};


auto UDPTextIO::receive(Request &t_req) const -> bool {

  std::string buffer(m_BUFFER_SIZE, '\0');

  sockaddr_in addr_in;
  socklen_t addr_len = sizeof(addr_in);

  ssize_t res = recvfrom(t_req.m_socket, buffer.data(), m_BUFFER_SIZE, 0,
      reinterpret_cast<struct sockaddr *>(&addr_in), &addr_len);

  t_req.m_valid =  is_valid(static_cast<int>(res), "UDPTextIO could not receive.");

 if(t_req.m_valid){

  t_req.m_address = Connection::address_tostring(addr_in)
    + ":"
    + Connection::port_tostring(addr_in); ;

  buffer.erase(std::find(buffer.begin(), buffer.end(), '\0'), buffer.end());
  t_req.set_data(new TextData(buffer));
 }

  return t_req.m_valid; 
};
