#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "active_conn.hpp"
#include "passive_conn.hpp"
#include "logger.hpp"
#include "data.hpp"
#include "text_data.hpp"

#include <vector>
#include <thread>

/* Public */

Client::Client(int t_port) : m_server_conn(t_port, new TextIO()) , m_incoming(4000, new TextIO()) {
   auto ip = std::string("127.0.0.1");
   m_server_req = m_server_conn.connect_socket(ip);

   m_incoming.bind_and_listen(my_ip);
   // std::thread accepting_connections(m_incoming.accept_connection);
}

bool Client::ping() { return true; }

bool Client::server_send_request(std::string &t_cmd){

   m_server_req.set_data(new TextData(t_cmd));

   m_server_conn.respond(m_server_req);
   m_server_conn.receive(m_server_req);

   if ( false == m_server_req.m_valid ){
      LOG_INFO("Request is not valid\n");
   }

   return m_server_req.m_valid;
}

// bool Client::client_send_request(std::string& t_contact_name, Data *t_data){

// }

// bool Client::client_receive_request()