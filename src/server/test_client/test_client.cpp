#include <iostream>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT

#include "active_conn.hpp"
#include "doctest.h"
#include "request.hpp"
#include "text_data.hpp"
#include "text_io.hpp"
#include "string_utils.hpp"
#include "user_chat.hpp"
#include "logger.hpp"

void chat_get_pending(std::string &response){
  auto [code, res] = StringUtils::split_first(response);

  if(code != "201"){
    LOG_ERR("%s", response.c_str());
    return;
  }

  auto [header, content] = StringUtils::split_first(res, ":");
  StringUtils::IntVector positions = StringUtils::split_to_ints(header, ",");

  StringUtils::StringVector chats = StringUtils::split_at(content, positions);

  std::vector<UserChat> user_chats;

  for(auto &chat : chats) {
     UserChat user_chat;
     user_chat.from_string(chat); // this methods loads string from server into the object.
     user_chats.push_back(user_chat);
  }

  for(auto &user_chat : user_chats ){
    //    std::cout << user_chat.id() << std::endl;  // the id of the chat
    //   std::cout << user_chat.created_at() << std::endl;  // when the chat was created (according to server timezone.)
       std::cout <<  user_chat.sender() << std::endl;  // the ID of the sender of the message.
       std::cout << user_chat.recipient() << std::endl;  // the ID of the recipient
   //    std::cout << user_chat.delivered() << std::endl;  // has the message been delivered to the recipient?
   //    std::cout << user_chat.text() << std::endl; // the actual chat text.
   //    std::cout << user_chat.text_length() << std::endl; // the length of the chat text.
  //   std::cout << user_chat.empty() << std::endl; // is the UserChat object empty?
   //  std::cout << user_chat.to_string() << std::endl; // convert a userChat object to string with a `,` as delimiter.
  }
}


int main(void) {

  ActiveConn conn(5000, new TextIO());

  std::cout << "attempting to connect to server..." << std::endl;
// >>>>>>> client_to_server
  //std::string addr = "206.189.0.154";
  std::string addr = "127.0.0.1";
  Request req = conn.connect_socket(addr);

  if (!req.m_valid) {
    std::cout << "Server is not running on " + addr +
                     ". Make sure you start the server before running this "
                     "tester app."
              << std::endl;
  }

  else {
      std::cout << "Connected. Waiting for server response..." << std::endl;
      conn.receive(req);
      std::string handshake = TextData::to_string(req.data());
      std::cout << handshake << std::endl;

    for (std::string line; std::getline(std::cin, line);) {

      auto[c, _] = StringUtils::split_first(line);
       
      
      if (line == "quit") {
        req.set_data(new TextData("EXIT"));
        conn.respond(req);
        break;
      }

      req.set_data(new TextData(line));
      conn.respond(req);
      conn.receive(req);

      std::string response = TextData::to_string(req.data());

      if(c == "CHAT"){
        chat_get_pending(response);
        continue;
      }
    
      std::cout << response << std::endl;
    };
  }
}

