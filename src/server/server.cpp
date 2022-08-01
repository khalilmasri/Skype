#include "server.hpp"

Server::Server(int t_port): m_conn(t_port, new TextIO()), m_address(SERVER_ADDRESS){
    m_conn.setup(m_address);
    m_conn.bind_and_listen(m_address);
  }

void Server::main_loop(){

   m_conn.accept_connection();
   Request req;

   if(m_conn.receive(req)){


   }
}
