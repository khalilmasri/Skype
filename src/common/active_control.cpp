#include "active_control.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

bool ActiveControl::connect_socket(int t_socket, sockaddr_in t_addr){
  auto *addr_ref   = reinterpret_cast<struct sockaddr *>(&t_addr);
   
  return connect(t_socket, addr_ref , sizeof(t_addr));
 
}
