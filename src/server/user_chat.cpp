#include "user_chat.hpp"

UserChat::UserChat(User &t_sender, User &t_recipient, std::string &t_text,
                   bool t_delivered)
    : m_id(0), m_sender(t_sender.id()), m_recipient(t_recipient.id()),
      m_text(t_text), m_delivered(t_delivered), m_empty(false) {}

bool UserChat::empty() const { return m_empty; }
int UserChat::sender() const { return m_sender; }
int UserChat::recipient() const { return m_recipient; }
std::string UserChat::text() const { return m_text; }
bool UserChat::delivered() const { return m_delivered; }
