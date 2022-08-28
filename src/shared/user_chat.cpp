#include "user_chat.hpp"
#include "string_utils.hpp"
#include <iostream>
#include "logger.hpp"

const std::string UserChat::m_FIELD_DELIM =",";
const std::string UserChat::m_HEADER_DELIM =":";

UserChat::UserChat()
    : m_id(0),
    m_sender(0),
    m_recipient(0),
    m_delivered(false),
    m_empty(true)
{}

UserChat::UserChat(const User &t_sender,
                   const User &t_recipient,
                   std::string &t_text,
                   bool t_delivered )
    : m_id(0),
    m_sender(t_sender.id()),
    m_recipient(t_recipient.id()),
    m_text(t_text),
    m_delivered(t_delivered),
    m_empty(false)
{}

UserChat::UserChat(int t_id,
                   std::string &t_created_at,
                   const User &t_sender,
                   const User &t_recipient,
                   std::string &t_text,
                   bool t_delivered)
    : m_id(t_id),
    m_created_at(t_created_at),
    m_sender(t_sender.id()),
    m_recipient(t_recipient.id()),
    m_text(t_text),
    m_delivered(t_delivered),
    m_empty(false)
{}

int UserChat::id() const { return m_id; }
bool UserChat::empty() const { return m_empty; }
int UserChat::sender() const { return m_sender; }
int UserChat::recipient() const { return m_recipient; }
std::string UserChat::created_at() const { return m_created_at; }
std::string UserChat::text() const { return m_text; }
bool UserChat::delivered() const { return m_delivered; }
std::size_t UserChat::text_length() const { return m_text.size(); }

std::string UserChat::to_string() const {
  return std::to_string(m_id) + m_FIELD_DELIM +
         m_created_at + m_FIELD_DELIM +
         std::to_string(m_sender) + m_FIELD_DELIM +
         std::to_string(m_recipient) + m_FIELD_DELIM +
         std::string(m_delivered ? "true" : "false") +
         m_text;
}

void UserChat::from_string(std::string &t_chat) {
   auto[header, content] = StringUtils::split_first(t_chat, m_HEADER_DELIM);

   std::size_t content_length = std::stoi(header);
   auto[info, text] = StringUtils::split_at(content, content_length);

   try {
   StringUtils::StringVector fields = StringUtils::split(info, m_FIELD_DELIM);

   m_id = std::stoi(fields.at(0));
   m_created_at = fields.at(1);
   m_sender = std::stoi(fields.at(2));
   m_recipient = std::stoi(fields.at(3));
   m_delivered = fields.at(4) == "true";
   m_text = text;

   } catch(...){
     LOG_ERR("Error Attempting to load a UserChat from:  %s", t_chat.c_str());
   }
}
