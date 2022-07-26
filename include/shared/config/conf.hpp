#ifndef CONF_H
#define CONF_H

#include <map>
#include <string>

class Config {
   static Config* m_instance;
   std::string const m_file_name = "../config/config.conf";

public:
   static Config& getInstance();

   Config();

   enum Configuration {
      SERVER_IP,
      SERVER_PORT,
      CLIENT_IP,
      CLIENT_PORT
   };
   
   std::string& toString(Configuration&); // enum to string

private:
   // Config(Config const&);
   // void operator=(Config const&);

   std::map<Configuration, std::string> m_conf_table;

public:
   // Config(Config const&)         = delete;
   // void operator=(Config const&) = delete;

   bool create(); // Creates a new config.conf if there is no file / or gets it from server later
   bool load(); // Loads config.conf
   bool save(); // Save config.conf 
};

#endif