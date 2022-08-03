#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "active_conn.hpp"
#include "passive_conn.hpp"
#include "request.hpp"
#include "text_data.hpp"

#include <vector>

/* Public */

Client::Client(int t_port) : server_conn(t_port, new TextIO()){
   auto ip = std::string("127.0.0.1");
   req = server_conn.connect_socket(ip);
}

bool Client::ping() { return true; }


/* Contact direct */
std::vector<std::string> Client::contact_get_contacts() const {
   return m_contacts.get_contacts();
}

bool Client::contact_list() {
   return m_contacts.list(server_conn, req);
}

bool Client::contact_search(std::string& t_cmd) {
   req.set_data(new TextData(t_cmd));
   return m_contacts.search(server_conn, req);
}

bool Client::contact_add_user(std::string& t_cmd) {
   req.set_data(new TextData(t_cmd));
   return m_contacts.add_user(server_conn, req);
}

bool Client::contact_remove_user(std::string& t_cmd) {
   return m_contacts.remove_user(t_cmd);
}

bool Client::contact_available(std::string& t_cmd) {
   req.set_data(new TextData(t_cmd));
   return m_contacts.available(server_conn, req);
}

/* User direct */
bool Client::user_set_username(std::string& t_username){
   return m_user.set_username(t_username);
}

bool Client::user_set_password(std::string& t_password){
   return m_user.set_password(t_password);
}

bool Client::user_register_user(){
   return m_user.register_user(server_conn, req);
}

bool Client::user_login() {
   return m_user.login(server_conn, req);
}

std::string Client::user_get_username() const {
   return m_user.get_username();
}

bool Client::user_get_logged_in() const {
   return m_user.get_logged_in();
}