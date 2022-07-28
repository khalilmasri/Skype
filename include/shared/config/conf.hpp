#ifndef CONF_H
#define CONF_H

#include <map>
#include <string>
#include <iostream>

class Config {
private:
   static Config* m_instance;

public:
   
   static Config* get_instance();
   
   enum ValidationLog { 
      Info, 
      Error, 
      Debug, 
      Critical 
   };

   enum Configuration {
   SERVER_IP,
   SERVER_PORT,
   CLIENT_IP,
   CLIENT_PORT,
   LAST
   };

   std::string    to_string(Configuration t_value) const; // enum to string
   std::string    parse_line(std::string& t_line); // Parses the lines from the conf files
   
   bool           is_valid(int t_result, const char *t_msg, ValidationLog t_log = Error) const;

   bool           create(); // Creates a new config.conf if there is no file / or gets it from server later
   bool           load(); // Loads config.conf
   bool           update_variable(Configuration t_variable, std::string t_value); // Update existing variables
   
   bool           corrupted_conf(); // Signal to the class that there is a corrupted conf file
   bool           corrupted_variable(Configuration t_variable); // Signal to the class that there is a corrupted variable so we load default
   
   template<typename T, typename B>
   void           print_table(std::map<T, B>& t_table); // To print table 

private:
   std::string const m_base_conf = "../config/config.conf";
   std::string m_user_conf;
   std::string m_user_dir;

   bool           compare_files();

   /* Conf table */
   std::map<Configuration, std::string> m_conf_table;
   
   void           init_conf_table(); // If map is empty initiate it.
   bool           append_to_map(std::string& t_line); // Append variable and value to map
   void           copy_conf();  // copy base config to user config

   Config();

};

// Config* Config::m_instance = nullptr;

#endif