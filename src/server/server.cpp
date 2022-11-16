#include "server.hpp"
#include "udp_text_io.hpp"
#include "reply.hpp"
#include "text_data.hpp"
#include "config.hpp"
#include <thread>

static Config *config = Config::get_instance();

Server::Server(int t_port)
    : m_conn(t_port, new TextIO()), m_udp(config->get<int>("UDP_PORT"), new UDPTextIO()),
      m_address(config->get<const std::string>("SERVER_ADDRESS")) {
  m_conn.setup();
  m_conn.bind_and_listen(m_address);

  m_udp.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
}
void Server::main_loop() {

  while (true) {
    accept_connection(); // checks for new connection at every iteration
                         
    Request req;

    m_conn.receive(req);
    m_router.route(req);
    m_conn.respond(req);
    disconnect_client_on_request(req);
    disconnect_client_on_failure(req);
  }
}

void Server::spawn_udp_listener() {
  std::thread udp_thread([this]() { this->udp_worker(); });
  udp_thread.detach();
}

void Server::accept_connection() {

  Request req = m_conn.accept_connection();

  if (req.m_valid) {

    LOG_TRACE("Accepted new connection. Socket fd: %d, address: %s", req.m_socket, req.m_address.c_str());
    std::string reply = Reply::get_message(Reply::r_200);
    req.set_data(new TextData(reply));
    m_conn.respond(req);
  }
}

void Server::disconnect_client_on_request(Request &t_req) {
  if (t_req.m_exit) {

    LOG_INFO("Disconnecting %s. Goodbye.", t_req.m_address.c_str());
    m_conn.disconnect_client(t_req);
  }
}

void Server::disconnect_client_on_failure(Request &t_req) {

  //  valid receives will always yield m_socket >= 0
  if (!t_req.m_valid && t_req.m_socket >= 0) {
    LOG_ERR("Client %s communication failed. Closing connection...",
            t_req.m_address.c_str());

    std::string empty;
    UserControllers::exit(empty, t_req); // forcefully calls exist to update database.

    m_conn.disconnect_client(t_req);
  }
}

void Server::udp_worker() {

  while (true) {
    Request req;
    req.m_valid = true;
    m_udp.receive(req);
    m_router.route(req);
    m_udp.respond(req);
  }
}
