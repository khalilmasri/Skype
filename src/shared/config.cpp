#include "config.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "string_utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

Config *Config::m_instance = nullptr;

Config::Config() {

  load_defaults();

  if (fs::exists(m_PATH)) {

    const std::string contents = read_config_file();

    load_config_contents(contents);

  } else {
    LOG_ERR(
        "Could not find the .sype.conf file in the path %s.  Loading defaults",
        CONFIG_PATH);
  }
}

Config *Config::get_instance() {

  if (m_instance == nullptr) {
    m_instance = new Config();
  }

  return m_instance;
}
void Config::free_instance() {

  if (m_instance != nullptr) {
    delete m_instance;
  }

  m_instance = nullptr;
}

const std::string Config::get(const std::string &t_key) const {

  if(key_exists(t_key)){
      return m_config.at(t_key);
  } else {
    LOG_ERR("Config key %s does not exists.", t_key.c_str());

    return {}; // returns an empty string if invalid key
  }
}


const std::string Config::get(const char *t_key) const {
   std::string key(t_key);
   return Config::get(key);
}

/***************************************************/
/************** CONFIG DEFAULTS HERE  *************/


void Config::load_defaults() {
  m_config = {
      {"TCP_PORT", "5000"},
      {"UDP_PORT", "7000"},
      {"SERVER_ADDRESS", "127.0.0.1"},
      {"HEADER_LENGTH", "10"},
      {"DB_USER", "postgres"},
      {"DB_PASSWORD", "postgres"},
      {"DB_ADDRESS", "localhost"},
      {"DB_NAME", "skype"},
      {"DB_PORT", "5432"},
  };
}
/**************************************************/
/**************************************************/

std::string Config::read_config_file() const{
  std::ifstream file(m_PATH, std::ios::in | std::ios::binary);

  if (!fs::is_regular_file(m_PATH)) {
    LOG_ERR("%s is an invalid config file. Could not load.", CONFIG_PATH);
    return {};
  }

  if (!file.is_open()) {
    LOG_ERR("Fail openning file stream.");
    return {};
  }

  std::string content{std::istreambuf_iterator<char>(file),
                      std::istreambuf_iterator<char>()};
  file.close();

  return content;
}

void Config::load_config_contents(const std::string &t_contents) {

  // std::cout << "config contents: " << t_contents << std::endl;
  StringUtils::StringVector split_string = StringUtils::split(t_contents, "\n");

  for (auto &val : split_string) {

    StringUtils::trim(val); // trim modifies string in place

    if (val.empty() || val.at(0) == '#') { // skip comments and empty lines
      continue;
    }
    auto [key, pair] = StringUtils::split_first(val, CONFIG_DELIM);

    StringUtils::trim(key);
    StringUtils::trim(pair);

    if (key_exists(key)) {
      m_config[key] = std::move(pair);
    } else {
      LOG_ERR("Could not load config at key '%s' because it is invalid.",
              key.c_str());
    }
  }
}


bool Config::key_exists(const std::string &t_key) const{
  return m_config.find(t_key) != m_config.end();
}


TEST_CASE("Config singleton") {
  Config *config = Config::get_instance();

  CHECK("localhost" == config->get("DB_ADDRESS"));
  CHECK(config->get("BAD").empty()); // returns empty on bad key
}
