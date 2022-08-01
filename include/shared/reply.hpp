#ifndef REPLY_H
#define REPLY_H
#include <string>

#include <unordered_map>

class Reply {

public:
  enum code { r_200, r_201, r_300, r_301, r_500, r_501 };
  static std::string get_message(const code t_code);
  static std::string append_message(const code t_code, const std::string &t_msg);

private:
  const static std::unordered_map<code, std::string> m_replies;
};

#endif // !REPLY_H
