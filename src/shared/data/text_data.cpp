#include "text_data.hpp"
#include "string_utils.hpp"
#include <string.h>
#include "doctest.h"

TextData::TextData(std::string &t_text) : m_size(t_text.size()){
  load_text(t_text);
};

TextData::TextData(std::string &&t_text) : m_size(t_text.size()){
  load_text(t_text);
};

TextData::TextData(const char *t_text) : TextData(std::string(t_text)) {}
TextData::TextData(char *t_text) : TextData(std::string(t_text)) {}

Data::type TextData::get_type() const { return m_type; }

bool TextData::empty() const { return m_size == 0; }

std::size_t TextData::size() const { return m_size; }

std::byte *TextData::get_data() const { return m_data; }

std::string TextData::to_string(const Data *t_data) {

  std::byte *data = t_data->get_data();

  std::string buffer;

  for (std::size_t i = 0; i < t_data->size(); i++) {
    buffer += std::to_integer<uint8_t>(data[i]);
  }

  return buffer;
}

TextData::~TextData() { delete[] m_data; }

void TextData::load_text(std::string &t_text) {

  m_data = new std::byte[m_size];
  std::size_t index = 0;

  for (const char c : t_text) {
    m_data[index] = static_cast<std::byte>(c);
    index++;
  }
}

TEST_CASE("Text Data"){

  auto data_from_const_char = new TextData("Hello world!");
  auto data_from_moved_str = new TextData(std::string("hello there!"));

  std::string str("hello friends!");
  char buffer[] = "C buffers!";

  auto data_from_str= new TextData(str);
  auto data_from_c_str = new TextData(buffer);

  CHECK(TextData::to_string(data_from_const_char) == "Hello world!");
  CHECK(TextData::to_string(data_from_moved_str) == "hello there!");
  CHECK(TextData::to_string(data_from_str) == str);
  CHECK(TextData::to_string(data_from_c_str) == buffer);
  CHECK(data_from_str->get_type() == Data::Text);
  CHECK(data_from_str->size() == str.size());
  CHECK(data_from_str->empty() == false);

  delete data_from_const_char;
  delete data_from_moved_str;
  delete data_from_str;
  delete data_from_c_str;

}




















// m_data = std::byte<char*>(buffer);

// auto [command, argument] = StringUtils::split_first(t_control_data);

// m_command = ServerCommand::get(command);
// m_arguments = std::move(argument);
// Data::type TextData::get_type() const { return m_type; };
//
// ServerCommand::name ControlData::command() const { return m_command; }
//
// std::string ControlData::reply() const { return Reply::get_message(m_reply);
// }
//
// std::string ControlData::reply(std::string &t_msg) const {
//   return Reply::append_message(m_reply, t_msg);
// }
//
// void ControlData::set_reply(Reply::code t_reply) { m_reply = t_reply; }
