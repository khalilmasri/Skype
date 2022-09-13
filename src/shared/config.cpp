#include "config.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "string_utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <type_traits>

Config *Config::m_instance = nullptr;

Config::Config() {

  load_defaults();

  if (fs::exists(m_PATH)) {

    const std::string contents = read_config_file();

    load_config_contents(contents);

  } else {
    LOG_ERR(
        "Could not find the .skype.conf file in your $HOME path '%s'.  Loading defaults",
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

template <typename T> T Config::get(const char *t_key) const {
  if (key_exists(t_key)) {

    if constexpr (std::is_same_v<T, const std::string>) {
      return get_str(t_key);

    } else if constexpr (std::is_same_v<T, int>) {
      return get_int(t_key);
    } else {
      LOG_ERR(
          "Invalid configuration type. Only const strings and ints allowed");
      return {};
    }

  } else {
    LOG_ERR("Config key %s does not exists.", t_key);
    return {}; // returns an empty string if invalid key
  }
}

const std::string Config::get_db() const {
  return std::string("postgresql://") + m_config.at("DB_USER") + ":" +
         m_config.at("DB_PASSWORD") + "@" + m_config.at("DB_ADDRESS") + ":" +
         m_config.at("DB_PORT") + "/" + m_config.at("DB_NAME");
}

/************** CONFIG DEFAULTS IN HEADER  *************/

void Config::load_defaults() {
  m_config = Config::m_DEFAULTS;
}

/*****************************************************/

std::string Config::read_config_file() const {
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

    set_server_address();
}

void Config::set_server_address() {
  // CHANGES THE DEFAULT SERVER_ADDRESS DEPENDING ON ENV.
  if (m_config.at("ENV") == "PROD") {
    m_config["SERVER_ADDRESS"] = m_config.at("REMOTE_IP");
  } else {
    m_config["SERVER_ADDRESS"] = m_config.at("LOCAL_IP");
  }
}

bool Config::key_exists(const std::string &t_key) const {
  return m_config.find(t_key) != m_config.end();
}

int Config::get_int(const char *t_key) const {

  try {
    return std::stoi(m_config.at(t_key));
  } catch (...) {

    LOG_ERR("Attempted to get a config key as integer but value is not valid",
            t_key);
    return -1;
  }
}

const std::string Config::get_str(const char *t_key) const {
  return m_config.at(t_key);
}

TEST_CASE("Config singleton") {
  Config *config = Config::get_instance();

  CHECK("localhost" == config->get<const std::string>("DB_ADDRESS"));
  CHECK(5000 == config->get<int>("TCP_PORT"));
  CHECK(config->get<const std::string>("BAD")
            .empty());                 // returns empty on bad key
  CHECK(config->get<int>("BAD") == 0); // bad ints returns 0
}
