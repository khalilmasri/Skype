#include "reply.hpp"

const std::unordered_map<Reply::Code, std::string> Reply::m_replies {
  {r_200, "200 OK"},
  {r_201, "201"}, // appends
  {r_202, "202 Invalid token. Please login"},
  {r_203, "203 Awating"},
  {r_300, "300 Not OK"},
  {r_301, "301 User not found"},
  {r_302, "302 User already exist"},
  {r_303, "303 Recipient not found"},
  {r_304, "304 Sender not found"},
  {r_305, "305 Invalid chat id"},
  {r_306, "306 Already awaiting. Please HANGUP before connecting"},
  {r_307, "307 Connection requests not found for this user"},
  {r_500, "500 Internal server error"},
  {r_501, "501 Invalid command or wrong number of arguments"},
  {r_502, "502 Empty argument"}, 
};

std::string Reply::get_message(const Code t_code){
   return m_replies.at(t_code);
}

std::string Reply::append_message(Code t_code, const std::string &t_msg){
   return m_replies.at(t_code) + " " + t_msg;
}
