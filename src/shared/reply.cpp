#include "reply.hpp"
#include "logger.hpp"

const std::unordered_map<Reply::Code, std::string> Reply::m_replies {
  {r_200, "200 OK"},
  {r_201, "201"}, // appends
  {r_202, "202 Invalid token. Please login"},
  {r_203, "203 Awaiting"},
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

const std::unordered_map<std::string, Reply::Code> Reply::m_codes {
  {"200" ,r_200},
  {"201", r_201}, 
  {"202" ,r_202},
  {"203" ,r_203},
  {"300" ,r_300},
  {"301" ,r_301},
  {"302" ,r_302},
  {"303" ,r_303},
  {"304" ,r_304},
  {"305" ,r_305},
  {"306" ,r_306},
  {"307" ,r_307},
  {"500" ,r_500},
  {"501" ,r_501},
  {"502" ,r_502}, 
};

std::string Reply::get_message(const Code t_code){
  try {
   return m_replies.at(t_code);
  } catch(...){
      LOG_ERR("Reply::Code %d does not exist");
      return {};
  }
}

std::string Reply::append_message(Code t_code, const std::string &t_msg){
   return m_replies.at(t_code) + " " + t_msg;
}

Reply::Code Reply::from_string(const std::string &t_code){
  try {
   return m_codes.at(t_code);

  } catch(...){
    LOG_ERR("Invalid Reply::Code %s", t_code.c_str());
    return None;
  }
}
