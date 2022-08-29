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

#include <iostream>
#include <vector>

/* Public */
                                                                                                          /* header|content*/
const std::string Client::m_CHAT_HEADER_DELIM = ":"; // this splits header from content chat content. e.g. 0,1,3,4:1,3,4,truesometext
const std::string Client::m_CHAT_FIELDS_DELIM = ","; // This splits the fields in chat e.g. 1,3,5,truesometext (chat_id, sender_id, recipient_id,text)

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


void Client::chat_send(Job &t_job){

   Request req;
   // SEND recipient_id message
   req.set_data(new TextData("SEND 4 hello there! how's life? :)"));
   server_conn.respond(req);
   server_conn.receive(req);
}

/* get pending chat message from a contactt to the current user (server will check the IP to define the current user.)
 *  example:
 *  PENDING 4
 *  where "4" is the id of contact which you are querying from pending chat messages.
 */
void Client::chat_get_pending(Job &t_job){

   Request req;
   req.set_data(new TextData("PENDING 4")); // ---->  GET CURRENT USER PENDING CHATS FROM A CONTACT ID = 4
   // req.set_data(new TextData("PENDING")); --->     GET ALL PENDING CHATS FROM CURRENT USER.
  
   server_conn.respond(req);
   server_conn.receive(req);

   std::string raw_response = TextData::to_string(req.data());
   auto [code, response ] = StringUtils::split_first(raw_response); // split the code out.
                                                                     
    if(code != "201"){
        // if is not 201 the server returned an error and you need to handle it.
        LOG_ERR("%s", raw_response.c_str());
        return;
    }

  /*  The response has has header like so -> header:content 
   *  the header has the position where to split the content between chat messages e.g. 0,34,56 (split at position 0, position 34, postion 56)
   *  Don't worry too much about the format of the response. Just use functions below and load the data into a UserChat.
   *  We dont have control over what characters users will type into messages so we cannot user delimiters.
   *  The length header allow us to specify where to split between the different chat messages.
   */

  auto [header, content] = StringUtils::split_first(response, m_CHAT_HEADER_DELIM);

  // grab positions from the header so we can split the content
  StringUtils::IntVector positions = StringUtils::split_to_ints(header, m_CHAT_FIELDS_DELIM);

  // split the the content between different chats.
  StringUtils::StringVector chats = StringUtils::split_at(content, positions);

  // loads all the chats into UserChat objects and put into a vector.
  std::vector<UserChat> user_chats;
  for(auto &chat : chats) {
     UserChat user_chat;
     user_chat.from_string(chat); // this methods loads string sent from server into the object. It will parse the string for you
     user_chats.push_back(user_chat);
  }

  // You can have access to the chat information easily but calling get methods on the object.

  for(auto &user_chat : user_chats ){
       std::cout << user_chat.id() << std::endl;  // the id of the chat
       std::cout << user_chat.created_at() << std::endl;  // when the chat was created (according to server timezone.)
       std::cout << user_chat.sender() << std::endl;  // the ID of the sender of the message.
       std::cout << user_chat.recipient() << std::endl;  // the ID of the recipient
       std::cout << user_chat.delivered() << std::endl;  // has the message been delivered to the recipient?
       std::cout << user_chat.text() << std::endl; // the actual chat text.
       std::cout << user_chat.text_length() << std::endl; // the length of the chat text.
       std::cout << user_chat.empty() << std::endl; // is the UserChat object empty?
       std::cout << user_chat.to_string() << std::endl; // convert a userChat object to string with a `,` as delimiter.
  }
}

// This is exactly as the object but you get all the chats from the current user.
//  The code to handle the response is exactly the same so you could use the same function for both send "CHAT" instead of "PENDING" to the server..
void Client::chat_get_all(Job &t_job){
   Request req;
   server_conn.respond(req);
   req.set_data(new TextData("CHAT 4")); 
   server_conn.receive(req);
   // .... then same as PENDING above.

}

bool Client::valid_response(Reply::Code t_code, std::string& t_res) {
   
   std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}
