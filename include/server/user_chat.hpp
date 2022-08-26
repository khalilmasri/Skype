#ifndef USER_CHAT_H
#define USER_CHAT_H

#include "user.hpp"

class UserChat{

  public:

  UserChat(User &t_sender, User &t_recipient, std::string &t_text, bool t_delivered = false);

  bool          empty() const;
  std::string   to_string() const;

  int           sender() const;
  int           recipient() const;
  std::string   text() const;
  bool          delivered() const;


  private:
  int           m_id;
  int           m_sender;
  int           m_recipient;
  std::string   m_text;
  bool          m_delivered;
  bool          m_empty;
};

#endif
