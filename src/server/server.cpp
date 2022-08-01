#include "server.hpp"
#include "text_data.hpp"

Server::Server(int t_port): m_conn(t_port, new TextIO()), m_address(SERVER_ADDRESS) {
    m_conn.setup(m_address);
    m_conn.bind_and_listen(m_address);
  }

void Server::main_loop(){

  while(true) {

    accept(); // checks for connections
    
    Request req;

    m_conn.receive(req);
    

  }
}


void Server::accept(){
  Request req  = m_conn.accept_connection();

  if(req.m_valid){
  std::string reply = Reply::get_message(Reply::r_200);
  req.set_data(new TextData(std::move(reply)));
  m_conn.respond(req);
  }

}

void Server::receive(Request &t_req){

}
