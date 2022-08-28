#include "reply.hpp"

const std::unordered_map<Reply::Code, std::string> Reply::m_replies {
  {r_200, "200 OK"},
  {r_201, "201"}, // appends
  {r_202, "202 Please login"},
  {r_300, "300 Not OK"},
  {r_301, "301 User not found"},
  {r_302, "302 User already exist"},
  {r_303, "303 Recipient not found"},
  {r_304, "304 Sender not found"},
  {r_500, "500 Internal server error"},
  {r_501, "501 Invalid command"},
  {r_502, "502 Empty argument"}, // appends
};

std::string Reply::get_message(const Code t_code){
   return m_replies.at(t_code);
}

std::string Reply::append_message(Code t_code, const std::string &t_msg){
   return m_replies.at(t_code) + " " + t_msg;
}
