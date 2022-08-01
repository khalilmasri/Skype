#include "reply.hpp"

const std::unordered_map<Reply::code, std::string> Reply::m_replies {
  {r_200, "200 OK"},
  {r_201, "201"},
  {r_300, "300 Not OK"},
  {r_301, "301 Not found"},
  {r_500, "500 Internal Server Error"},
  {r_501, "501 Invalid Command"},
};

std::string Reply::get_message(const code t_code){
   return m_replies.at(t_code) + "\n";
}

std::string Reply::append_message(code t_code, const std::string &t_msg){
   return m_replies.at(t_code) + " " + t_msg + "\n";
}
