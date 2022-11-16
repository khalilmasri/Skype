#include "text_data.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include <cstring>

TextData::TextData(std::string &t_text) {
  m_data.reserve(t_text.size());
  load_text(t_text);
};

TextData::TextData(std::string &&t_text) {
  m_data.reserve(t_text.size());
  load_text(t_text);
};

TextData::TextData(const char *t_text) : TextData(std::string(t_text)) {}
TextData::TextData(char *t_text) : TextData(std::string(t_text)) {}

TextData::~TextData() = default;

auto TextData::get_type() const -> Data::Type { return m_type; }
auto TextData::empty() const -> bool { return m_data.empty(); }
auto TextData::size() const -> std::size_t { return m_data.size(); }
auto TextData::get_data() const -> DataVector { return m_data; }
auto TextData::get_data_ref() const -> const Data::DataVector& { return m_data; }

/* */

auto TextData::to_string(const Data *t_data) -> std::string {

  DataVector data = t_data->get_data();
  std::string buffer;

  for (auto &character : data) {
    buffer += static_cast<char>(character);
  }

  return buffer;
}

/* */

void TextData::load_text(std::string &t_text) {
  for (char &character : t_text) {
    m_data.push_back(character);
  }
}

TEST_CASE("Text Data") {

  auto *data_from_const_char = new TextData("Hello world!");
  auto *data_from_moved_str = new TextData(std::string("hello there!"));

  std::string str("hello friends!");
  std::string buffer = "C buffers!";

  auto *data_from_str = new TextData(str);
  auto *data_from_c_str = new TextData(buffer);

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
