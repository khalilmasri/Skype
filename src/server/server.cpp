#include "server.hpp"
#include "reply.hpp"
#include "text_data.hpp"

Server::Server(int t_port)
    : m_conn(t_port, new TextIO()), m_address(SERVER_ADDRESS) {
  m_conn.setup(m_address);
  m_conn.bind_and_listen(m_address);
}

void Server::main_loop() {

  while (true) {
    accept_connection(); // look for new connection at every iteration

    Request req;
    m_conn.receive(req);
    m_router.route(req);
    m_conn.respond(req);
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
