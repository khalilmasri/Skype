#include "chat.hpp"
#include "user_chat.hpp"
#include "request.hpp"
#include "text_data.hpp"
#include "fail_if.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "logger.hpp"

#include <QString>
#include <QVector>
#include <string>
#include <iostream>

QVector<Chat> Chat::get_pending(ActiveConn& t_conn, Request &t_req) 
{  
  std::string command = "PENDING " + TextData::to_string(t_req.data());
  std::string response = "";
  std::string argument = TextData::to_string(t_req.data());

  t_req.set_data(new TextData(command));
  
  t_conn.respond(t_req);
  t_conn.receive(t_req);

  FAIL_IF( false == t_req.m_valid);
  response = TextData::to_string(t_req.data());

  FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

  return parse_chat(response);

fail:

  LOG_ERR("Getting pending chats failed");
  return {};
}

QVector<Chat> Chat::get_all(ActiveConn& t_conn, Request &t_req)
{
  std::string command = "CHAT " + TextData::to_string(t_req.data());
  std::string response = "";
  
  t_req.set_data(new TextData(command));

  t_conn.respond(t_req);
  t_conn.receive(t_req);

  FAIL_IF( false == t_req.m_valid);
  response = TextData::to_string(t_req.data());

  FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

  return parse_chat(response);

fail:

  LOG_ERR("Getting chat failed");
  return {};
}

bool Chat::send(ActiveConn& t_conn, Request &t_req)
{
  std::string command = "SEND " + TextData::to_string(t_req.data());
  std::string response = "";

  t_req.set_data(new TextData(command));
  
  t_conn.respond(t_req);
  t_conn.receive(t_req);

  FAIL_IF( false == t_req.m_valid);

  response = TextData::to_string(t_req.data());

  FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

  return true;

fail:

  LOG_ERR("Couldn't send message");
  return false;
}

bool Chat::deliver(ActiveConn& t_conn, Request &t_req)
{
  std::string command = "DELIVERED " + TextData::to_string(t_req.data());
  std::string response = "";

  t_req.set_data(new TextData(command));
  
  t_conn.respond(t_req);
  t_conn.receive(t_req);

  FAIL_IF( false == t_req.m_valid);

  response = TextData::to_string(t_req.data());

  FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

  return true;

fail:

  LOG_ERR("Couldn't deliver that the messages were recieved!");
  return false;
}

// ***** PRIVATE ***** //

QVector<Chat> Chat::parse_chat(std::string &t_response)
{
  auto [code, response] = StringUtils::split_first(t_response);
  auto [header, content] = StringUtils::split_first(response, m_HEADER_DELIM); // :
  StringUtils::IntVector positions = StringUtils::split_to_ints(header, m_FIELD_DELIM); // ,
  StringUtils::StringVector chats = StringUtils::split_at(content, positions);
  
  QVector<Chat> user_chats;
 
  for(auto &chat : chats) {
     Chat user_chat;
     user_chat.from_string(chat);
     user_chats.push_back(user_chat);
  }

  return user_chats;
}

bool Chat::valid_response(Reply::Code t_code, std::string& t_res) {
   
  std::string code = Reply::get_message(t_code);
  
  auto found = t_res.find(code);
  if ( found != std::string::npos){
    return true;
  }
  
  return false;
}
