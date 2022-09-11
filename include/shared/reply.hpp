#ifndef REPLY_H
#define REPLY_H
#include <string>

#include <unordered_map>

class Reply {

public:
  enum Code { r_200, r_201, r_202, r_203, r_300, r_301, r_302, r_303, r_304, r_305, r_306, r_307, r_500, r_501, r_502, None };
  static std::string get_message(const Code t_code);
  static Code from_string(const std::string &t_code);
  static std::string append_message(const Code t_code, const std::string &t_msg);

private:
  const static std::unordered_map<Code, std::string> m_replies;
  const static std::unordered_map<std::string, Code> m_codes;
};

#endif // !REPLY_H
