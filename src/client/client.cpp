#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "active_conn.hpp"
#include "passive_conn.hpp"

#include <vector>
#include <thread>

/* Public */

Client::Client(int t_port) : server_conn(port, new TextIO()) {
   auto ip = std::string("127.0.0.1");
   server_req = server_conn.connect_socket(ip);

   m_incoming.bind_and_listen(my_ip);
   std::thread accepting_connections(m_incoming.accept_connection);
}

bool Client::ping() { return true; }

bool Client::server_send_request(std::string &t_cmd){

   req.set_data(new TextData(t_cmd));

   server_conn.respond(req);
   server_conn.receive(req);

   if ( false == req.m_valid ){
      LOG_INFO("Request is not valid\n");
   }

   return req.m_valid;
}

// bool Client::client_send_request(std::string& t_contact_name, Data *t_data){

// }

// bool Client::client_receive_request()