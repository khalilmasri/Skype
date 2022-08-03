#ifndef REPLY_H
#define REPLY_H
#include <string>

#include <unordered_map>

class Reply {

public:
  enum Code { r_200, r_201, r_300, r_301, r_500, r_501 };
  static std::string get_message(const Code t_code);
  static std::string append_message(const Code t_code, const std::string &t_msg);

private:
  const static std::unordered_map<Code, std::string> m_replies;
};

#endif // !REPLY_H
