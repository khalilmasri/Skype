#include "accounts.hpp"
#include "accounts.hpp"
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
#include "user_chat.hpp"
#include "string_utils.hpp"
#include "chat.hpp"

#include <iostream>
#include <vector>

/* Public */

Client::Client(){  

   std::string response = "";
   LOG_INFO("Connecting to server...");
   
   auto ip = std::string("206.189.0.154");
   m_server_req = m_server_conn.connect_socket(ip);

   m_server_conn.receive(m_server_req);
   
   FAIL_IF( false == m_server_req.m_valid);
   
   response = TextData::to_string(m_server_req.data());

   FAIL_IF_MSG( false == valid_response(Reply::r_200, response),"Client didn't connect to the server");

   LOG_INFO("%s! Client connected to server succesfully", response.c_str());

  return;

fail:
   exit(1);
}

Client::~Client(){
   LOG_INFO("Disconnecting from server");
      
   std::string command = "EXIT";
   m_server_req.set_data(new TextData(command));

   m_server_conn.respond(m_server_req);
   m_server_conn.receive(m_server_req);

   std::string response = TextData::to_string(m_server_req.data());
   LOG_INFO("Server reply => %s", response.c_str());

   close(m_server_conn.get_socket());

   LOG_INFO("Client disconnected\n");
}

/* Contact direct */

void Client::contact_get_contacts(Job &t_job) {
   LOG_TRACE("Getting contacts to display...")
   t_job.m_contact_list = m_contacts.display_contacts();

   if (false == t_job.m_contact_list.empty()){
      t_job.m_valid = true;
   }
   LOG_TRACE("Fetching display contacts is done!");
}

void Client::contact_list(Job &t_job) {
   t_job.m_valid = m_contacts.list(m_server_conn, m_server_req);
}

void Client::contact_search(Job &t_job) {
   LOG_TRACE("Searching for user");
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.search(m_server_conn, m_server_req);
}

void Client::contact_add_user(Job &t_job) {
   LOG_TRACE("Adding user");
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.add_user(m_server_conn, m_server_req);
}

void Client::contact_remove_user(Job &t_job) {
   LOG_TRACE("Removing user");
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.remove_user(m_server_conn,m_server_req);
}

void Client::contact_available(Job &t_job) {
   LOG_TRACE("Checking for user availability!");
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_contacts.available(m_server_conn, m_server_req);
}


/* User direct */
void Client::user_set_username(Job &t_job){
   LOG_TRACE("Setting username!");
   t_job.m_valid = m_user.set_username(t_job.m_argument);
}

void Client::user_set_password(Job &t_job){
   t_job.m_valid = m_user.set_password(t_job.m_argument);
}

void Client::user_register_user(Job &t_job){
   LOG_TRACE("Registering user!");
   t_job.m_valid = m_user.register_user(m_server_conn, m_server_req);
}

void Client::user_login(Job &t_job) {
   LOG_TRACE("Logging to user!");
   t_job.m_valid = m_user.login(m_server_conn, m_server_req);
}

void Client::user_get_username(Job &t_job){
   LOG_TRACE("Getting username!");
   t_job.m_string = m_user.get_username();

   if ("" != t_job.m_string){
      t_job.m_valid = true;
   }
}

void Client::user_get_logged_in(Job &t_job) {
   LOG_TRACE("Getting user log status!");
   t_job.m_valid = m_user.get_logged_in();
}

void Client::user_get_id(Job &t_job)
{
   LOG_TRACE("Getting user ID!");
   t_job.m_intValue = m_user.get_id();
   
   if ( -1 != t_job.m_intValue)
   {
      t_job.m_valid = true;
   }
}

// CHAT Methods

void Client::chat_send(Job &t_job){
   LOG_TRACE("Sending chat!");
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_chat.send(m_server_conn, m_server_req);
}

void Client::chat_get_pending(Job &t_job){
   LOG_TRACE("Getting pending chats...");
   m_server_req.set_data(new TextData(t_job.m_argument)); // ---->  GET CURRENT USER PENDING CHATS FROM A CONTACT ID = 4
   
   t_job.m_chats = m_chat.get_pending(m_server_conn, m_server_req);

   if (false == t_job.m_chats.empty())
   {
      t_job.m_valid = true;
   }
   LOG_TRACE("Getting pending chats is done!");
}

void Client::chat_get_all(Job &t_job){
   LOG_TRACE("Getting all chats...");
   m_server_req.set_data(new TextData(t_job.m_argument));

   t_job.m_chats = m_chat.get_all(m_server_conn, m_server_req);

   if (false == t_job.m_chats.empty())
   {
      t_job.m_valid = true;
   }
   LOG_TRACE("Getting all chats is done!");
}

void Client::chat_deliver(Job &t_job)
{
   m_server_req.set_data(new TextData(t_job.m_argument));
   t_job.m_valid = m_chat.deliver(m_server_conn, m_server_req);
}

bool Client::valid_response(Reply::Code t_code, std::string& t_res) {
   
   std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}
