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
#include "supress_unused_params_warnings.hpp"

#include <iostream>
#include <vector>

/* Public */

Client::Client(){  

   std::string response = "";
   LOG_INFO("Connecting to server...");
   
   auto ip = std::string("206.189.0.154");
   server_req = server_conn.connect_socket(ip);

   server_conn.receive(server_req);
   
   FAIL_IF( false == server_req.m_valid);
   
   response = TextData::to_string(server_req.data());

   FAIL_IF_MSG( false == valid_response(Reply::r_200, response),"Client didn't connect to the server");

   LOG_INFO("%s! Client connected to server succesfully", response.c_str());

  return;

fail:
   exit(1);
}

Client::~Client(){
   LOG_INFO("Disconnecting from server");
   
   JobBus::set_exit();
   
   std::string command = "EXIT";
   server_req.set_data(new TextData(command));

   server_conn.respond(server_req);
   server_conn.receive(server_req);

   std::string response = TextData::to_string(server_req.data());
   LOG_INFO("Server reply => %s", response.c_str());

   close(server_conn.get_socket());

   LOG_INFO("Client disconnected\n");
}

/* Contact direct */

void Client::contact_get_current_contact(Job &t_job) {
   t_job.m_string = m_contacts.get_current_contact();
   
   if ("" != t_job.m_string){
      t_job.m_valid = true;
   }
}

void Client::contact_set_current_contact(Job &t_job) {
   t_job.m_valid = m_contacts.set_current_contact(t_job.m_argument);
}

void Client::contact_get_contacts(Job &t_job) {
   t_job.m_vector = m_contacts.display_contacts();

   if (false == t_job.m_vector.empty()){
      t_job.m_valid = true;
   }
}

void Client::contact_list(Job &t_job) {
   t_job.m_valid = m_contacts.list(server_conn, server_req);
}

void Client::contact_search(Job &t_job) {
   server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.search(server_conn, server_req);
}

void Client::contact_add_user(Job &t_job) {
   server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.add_user(server_conn, server_req);
}

void Client::contact_remove_user(Job &t_job) {
   server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.remove_user(server_conn,server_req);
}

void Client::contact_available(Job &t_job) {
   server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.available(server_conn, server_req);
}


/* User direct */
void Client::user_set_username(Job &t_job){
   t_job.m_valid = m_user.set_username(t_job.m_argument);
}

void Client::user_set_password(Job &t_job){
   t_job.m_valid = m_user.set_password(t_job.m_argument);
}

void Client::user_register_user(Job &t_job){
   t_job.m_valid = m_user.register_user(server_conn, server_req);
}

void Client::user_login(Job &t_job) {
   t_job.m_valid = m_user.login(server_conn, server_req);
}

void Client::user_get_username(Job &t_job){
   t_job.m_string = m_user.get_username();

   if ("" != t_job.m_string){
      t_job.m_valid = true;
   }
}

void Client::user_get_logged_in(Job &t_job) {
   t_job.m_valid = m_user.get_logged_in();
}

bool Client::valid_response(Reply::Code t_code, std::string& t_res) {
   
   std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}