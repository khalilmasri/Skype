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

UserChat::UserChat(int t_id,
                   std::string &t_created_at,
                   const int t_sender_id,
                   const int t_recipient_id,
                   std::string &t_text,
                   bool t_delivered)
    : m_id(t_id),
    m_created_at(t_created_at),
    m_sender(t_sender_id),
    m_recipient(t_recipient_id),
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

void UserChat::set_delivered(bool t_delivered) { m_delivered = t_delivered ;}

std::string UserChat::created_at_date() const {
   auto[date, _] = StringUtils::split_first(m_created_at);
   return date;
}

std::string UserChat::created_at_time() const {
    auto [_, time_long] = StringUtils::split_first(m_created_at);
    auto [hours, time_rest] = StringUtils::split_first(time_long, ":");
    auto [minutes, __] = StringUtils::split_first(time_rest, ":");
    std::string time = hours + ":" + minutes;
   return time;
}


bool UserChat::operator<(UserChat &rhs){
   auto[date_left, time_left] = StringUtils::split_first(m_created_at);
   auto[date_right, time_right] = StringUtils::split_first(rhs.created_at());

   return compare_dates(date_left, date_right, LesserThan) && compare_times(time_left, time_right , LesserThan);
}

bool UserChat::operator>(UserChat &rhs){
   auto[date_left, time_left] = StringUtils::split_first(m_created_at);
   auto[date_right, time_right] = StringUtils::split_first(rhs.created_at());

   return compare_dates(date_left, date_right, GreaterThan) && compare_times(time_left, time_right , GreaterThan);
}

std::string UserChat::to_string() const {
  return std::to_string(m_id) + m_FIELD_DELIM + m_created_at + m_FIELD_DELIM +
         std::to_string(m_sender) + m_FIELD_DELIM +
         std::to_string(m_recipient) + m_FIELD_DELIM +
         std::string(m_delivered ? "true" : "false") + m_text;
}

template <typename T> T UserChat::get_field(std::string &t_field) {

  if (t_field == "id") {
    return m_id;
  }

  if (t_field == "sender") {
    return m_sender;
  }

  if (t_field == "recipient") {
    return m_recipient;
  }

  if (t_field == "created_at") {
    return m_created_at;
  }

  if (t_field == "delivered") {
    return m_delivered;
  }

  LOG_ERR("User chat '%s' does not exist.", t_field.c_str());
  return -1;

}

void UserChat::from_string(std::string &t_chat) {
  auto [header, content] = StringUtils::split_first(t_chat, m_HEADER_DELIM);

  
  try {
    std::size_t content_length = std::stoi(header);
    auto [info, text] = StringUtils::split_at(content, content_length);

    StringUtils::StringVector fields = StringUtils::split(info, m_FIELD_DELIM);

    m_id = std::stoi(fields.at(0));
    m_created_at = fields.at(1);
    m_sender = std::stoi(fields.at(2));
    m_recipient = std::stoi(fields.at(3));
    m_delivered = fields.at(4) == "true";
    m_text = text;

  } catch (...) {
    LOG_ERR("Error Attempting to load a UserChat from:  %s", t_chat.c_str());
  }
}

bool UserChat::compare_dates(std::string &t_lfs, std::string &t_rhs, ComparisonType t_comp_type){

     StringUtils::StringVector left = StringUtils::split(t_lfs, "-");
     StringUtils::StringVector right = StringUtils::split(t_rhs, "-");

     if(left.size() != right.size()){
       LOG_ERR("Bad date comparison '%s' and '%s'. Dates must have the same number o fields.", t_lfs.c_str(), t_rhs.c_str());
       return false;
      }

     return compare(left, right, t_comp_type);
}

bool UserChat::compare_times(std::string &t_lfs, std::string &t_rhs, ComparisonType t_comp_type){

     const int SECS = 2;
     StringUtils::StringVector left = StringUtils::split(t_lfs, ":");
     StringUtils::StringVector right = StringUtils::split(t_rhs, ":");

     if(left.size() != 3 || right.size() != 3){
       LOG_ERR("Bad time comparison '%s' and '%s'. Time must have 3 fields HH:MM:SS.mm.", t_lfs.c_str(), t_rhs.c_str());
       return false;
      }

     auto[left_secs, left_milli] = StringUtils::split_first(left[SECS], ".");
     left[SECS] = left_secs;
     left.push_back(left_milli);

    auto[right_secs, right_milli] = StringUtils::split_first(right[SECS], ".");
     right[SECS] = right_secs;
     right.push_back(right_milli);

     return compare(left, right, t_comp_type);
}


bool UserChat::compare(std::vector<std::string> &t_lhs, std::vector<std::string> &t_rhs, ComparisonType t_comp_type) {

     for(std::size_t i = 0; i < t_lhs.size(); i++ ){
         try {
           int l = std::stoi(t_lhs[i]);
           int r = std::stoi(t_rhs[i]);

           if(t_comp_type == GreaterThan && l < r){
               return false;
           }

           if(t_comp_type == LesserThan && l > r){
               return false;
           }

         } catch(...){
          LOG_ERR("Bad date field '%s' and '%s'. It must be a number.", t_lhs[i].c_str(), t_rhs[i].c_str());
          return false;
         }
     }

      return true;
}

