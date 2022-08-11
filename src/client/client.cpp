#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "active_conn.hpp"
#include "passive_conn.hpp"
#include "request.hpp"
#include "text_data.hpp"
#include "logger.hpp"
#include "fail_if.hpp"
#include "job_bus.hpp"

#include <iostream>
#include <vector>

/* Public */

Client::Client(int t_port) : server_conn(t_port, new TextIO()){
   
   if ( t_port == 0 ) {
      return;
   }
   
   std::string response = "";
   LOG_INFO("Connecting to server...");

   auto ip = std::string("206.189.0.154");
   req = server_conn.connect_socket(ip);
   
   server_conn.receive(req);
   
   FAIL_IF( false == req.m_valid);
   
   response = TextData::to_string(req.data());

   FAIL_IF_MSG( false == valid_response(Reply::r_200, response),"Client didn't connect to the server");

   LOG_INFO("%s! Client connected to server succesfully", response.c_str());

   return;

fail:
   exit(1);
}

Client::~Client(){
   LOG_INFO("Disconnecting from server");
   
   // JobBus::set_exit();
   
   std::string command = "EXIT";
   req.set_data(new TextData(command));

   server_conn.respond(req);
   server_conn.receive(req);

   std::string response = TextData::to_string(req.data());
   LOG_INFO("Server reply => %s", response.c_str());

   close(server_conn.get_socket());

   LOG_INFO("Client disconnected\n");
}

bool Client::ping() { 
  req.set_data(new TextData("PING"));
   
   server_conn.respond(req);
   server_conn.receive(req);
   
   std::cout << "server reply => " << TextData::to_string(req.data());
   
   return true; 
}

/* Contact direct */
std::vector<std::string> Client::contact_get_contacts() {
   return m_contacts.display_contacts();
}

std::string Client::contact_get_current_contact() {
   return m_contacts.get_current_contact();
}

void Client::contact_set_current_contact(std::string &t_current_contact) {
   m_contacts.set_current_contact(t_current_contact);
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
   req.set_data(new TextData(t_cmd));
   return m_contacts.remove_user(server_conn,req);
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

bool Client::valid_response(Reply::Code t_code, std::string& t_res) {
   
   std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}