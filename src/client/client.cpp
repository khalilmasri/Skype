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
   
   // JobBus::set_exit();
   
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

void Client::contact_get_current_contact(std::string &t_arg, std::string &t_ret) {
   UNUSED_PARAMS(t_arg);
   t_ret = m_contacts.get_current_contact();
}

void Client::contact_set_current_contact(std::string &t_arg, bool t_ret) {
   t_ret = m_contacts.set_current_contact(t_arg);
}

void Client::contact_get_contacts(std::string &t_arg, std::vector<std::string> &t_ret) {
   UNUSED_PARAMS(t_arg);
   t_ret = m_contacts.display_contacts();
}

void Client::contact_list(std::string &t_arg, bool t_ret) {
   UNUSED_PARAMS(t_arg);
   t_ret = m_contacts.list(server_conn, server_req);
}

void Client::contact_search(std::string &t_arg, bool t_ret) {
   server_req.set_data(new TextData(t_arg));
   t_ret = m_contacts.search(server_conn, server_req);
}

void Client::contact_add_user(std::string &t_arg, bool t_ret) {
   server_req.set_data(new TextData(t_arg));
   t_ret = m_contacts.add_user(server_conn, server_req);
}

void Client::contact_remove_user(std::string &t_arg, bool t_ret) {
   server_req.set_data(new TextData(t_arg));
   t_ret = m_contacts.remove_user(server_conn,server_req);
}

void Client::contact_available(std::string &t_arg, bool t_ret) {
   server_req.set_data(new TextData(t_arg));
   t_ret = m_contacts.available(server_conn, server_req);
}


/* User direct */
void Client::user_set_username(std::string &t_arg, bool t_ret){
   t_ret = m_user.set_username(t_arg);
}

void Client::user_set_password(std::string &t_arg, bool t_ret){
   t_ret = m_user.set_password(t_arg);
}

void Client::user_register_user(std::string &t_arg, bool t_ret){
   UNUSED_PARAMS(t_arg);
   t_ret = m_user.register_user(server_conn, server_req);
}

void Client::user_login(std::string &t_arg, bool t_ret) {
   UNUSED_PARAMS(t_arg);
   t_ret = m_user.login(server_conn, server_req);
}

void Client::user_get_username(std::string &t_arg, std::string &t_ret){
   UNUSED_PARAMS(t_arg);
   t_ret = m_user.get_username();
}

void Client::user_get_logged_in(std::string &t_arg, bool t_ret) {
   UNUSED_PARAMS(t_arg);
   t_ret = m_user.get_logged_in();
}

bool Client::valid_response(Reply::Code t_code, std::string& t_res) {
   
   std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}