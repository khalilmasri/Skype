#include "server.hpp"
#include "reply.hpp"
#include "text_data.hpp"

Server::Server(int t_port)
    : m_conn(t_port, new TextIO()), m_udp(5001, new TextIO()), m_address(SERVER_ADDRESS) {
  m_conn.setup();
  m_conn.bind_and_listen(m_address);

  m_udp.setup(m_address);
  m_udp.bind_socket();

}
void Server::main_loop() {

  while (true) {
    accept_connection(); // searches for new connection at every iteration
                        
    do_udp();

    Request req;
    m_conn.receive(req);
    m_router.route(req);
    m_conn.respond(req);
    disconnect_on_client_request(req);
    disconnect_client_on_failure(req);
  }
}

void Server::accept_connection() {

  Request req = m_conn.accept_connection();

  if (req.m_valid) {
    std::string reply = Reply::get_message(Reply::r_200);
    req.set_data(new TextData(reply));
    m_conn.respond(req);
  }
}

void Server::disconnect_on_client_request(Request &t_req) {
  if (t_req.m_exit) {

    LOG_INFO("Disconnecting %s. Goodbye.", t_req.m_address.c_str());
    m_conn.disconnect_client(t_req);
  }
}

void Server::disconnect_client_on_failure(Request &t_req) {
  //  valid receives will always yields m_socket >= 0
  if (!t_req.m_valid && t_req.m_socket >= 0) {
    LOG_ERR("Client %s communication failed. Closing connection...",
            t_req.m_address.c_str());
 
    std::string empty;
    UserControllers::exit(empty, t_req); // forcefully calls exist to update database.

    m_conn.disconnect_client(t_req);
  }
}

void Server::do_udp(){

  Request req;
   m_udp.receive(req);
   std::cout << TextData::to_string(req.data()) << std::endl;
}
