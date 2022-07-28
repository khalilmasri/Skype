#include "conf.hpp"
#include "logger.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#if __linux__
namespace fs = std::filesystem;
#elif __APPLE__ || __MACH__
namespace fs = std::__fs::filesystem;
#endif

/* Public */

Config::Config() {

   m_user_dir = std::string(getenv("HOME")) + "/.config/my_skype";
   m_user_conf = m_user_dir + "/config.conf";

   // bool res = compare_files();
   bool res = fs::is_directory(m_user_dir);
   if ( false == res) {
      create();
   }

   res = load();
   if ( false == res ) {
      create();
      load();
   }

   print_table(m_conf_table);
}

Config* Config::get_instance() {
   
   if ( !m_instance ) {
      m_instance = new Config;
   }

   return m_instance;
}

bool Config::create(){

   LOG_INFO("Creating skype directory and config file");
   
   bool res = fs::is_directory(m_user_dir);
   if ( false == res ){
      res = fs::create_directory(m_user_dir);
   }
   
   if ( false == res ){
      return is_valid(res, "Couldn't create directory", Error);
   }
 
   copy_conf();

   return res;
}

bool Config::load(){

   std::ifstream file(m_user_conf);
   std::string line;
   
   int res = file.good();
   if ( true == res){

      LOG_INFO("Parsing config file");

      while (std::getline(file,line)){
         
         res = append_to_map(line);
         if ( false == res){
            return is_valid(res, "Parsing the config file failed!", Error);
         }

      }
      
      file.close();
   }
   else 
   {
      return is_valid(res, "Couldn't open file", Error);
   }


   return true;  
}

bool Config::update_variable(Configuration t_variable, std::string t_value) {

   bool res = (t_variable == SERVER_IP && t_variable < LAST) ? true : false;

   if ( false == res || t_value.empty() ) {
      return is_valid(res, "Couldn't update variable", Error);
   }

   m_conf_table.at(t_variable) = t_value;

   return true;
}

bool Config::corrupted_conf() {

   LOG_ERR("Corrupted config file, fixing it");
   
   bool res = create();
   if ( false == res ){
      return is_valid(res, "Could'nt fix config file", Error);
   }

   return true;
}

bool Config::corrupted_variable(Configuration t_variable){

   std::ifstream file(m_base_conf);
   std::string line;
   
   int res = file.good();
   if ( true == res){

      while (std::getline(file,line)){

         std::string variable = parse_line(line);
         if ( variable == to_string(t_variable)) {
            
            m_conf_table.at(t_variable) = line;
         }         
      }
      
      file.close();
   }
   else 
   {
      return is_valid(res, "Couldn't open file", Error);
   }


   return true; 

}
/* Private */

bool Config::compare_files(){

   std::ifstream base(m_base_conf, std::ifstream::ate | std::ifstream::binary);
   std::ifstream user(m_user_conf, std::ifstream::ate | std::ifstream::binary);
   
   const std::ifstream::pos_type base_size = base.tellg();
   const std::ifstream::pos_type user_size = user.tellg();

   if (user_size != base_size ){

      is_valid(false, "Config files have different sizes", Error);
   }

   
   base.seekg(0);
   user.seekg(0);
   
   std::istreambuf_iterator<char> base_begin(base);  
   std::istreambuf_iterator<char> user_begin(user);
  
   return std::equal(base_begin, std::istreambuf_iterator<char>(), user_begin);
}

bool Config::append_to_map(std::string& t_line){

   if ( m_conf_table.empty() ){
      init_conf_table();
   }

   std::string variable = parse_line(t_line);
   std::string value = t_line;

   if ( variable.empty() || value.empty() || variable == value ){
      return false;
   }

   for (int conf_int = SERVER_IP; conf_int != LAST; conf_int++) {

      std::string conf = to_string(Configuration(conf_int));
      if ( variable == conf ){
         m_conf_table.at(Configuration(conf_int)) = value;
         return true;
      }      
   }

   return false;
}

void Config::init_conf_table() {

   for (int conf_int = SERVER_IP; conf_int != LAST; conf_int++) {
      m_conf_table.emplace(Configuration(conf_int), "");
   }
}

std::string Config::parse_line(std::string& t_line){

   std::string delimiter = "=";
   std::string token = "";
   size_t pos = 0;

   pos = t_line.find(delimiter);

   if (pos != std::string::npos) {
      
      token = t_line.substr(0, pos);
      t_line.erase(0, pos + delimiter.length());

   }
      
   return token;
}

std::string Config::to_string(Configuration t_value) const{

   std::map<Configuration, std::string> table {
      {SERVER_IP,    "SERVER_IP"},
      {SERVER_PORT,  "SERVER_PORT"},
      {CLIENT_IP,    "CLIENT_IP"},
      {CLIENT_PORT,  "CLIENT_PORT"}
   };

   return table.at(t_value);
}

void Config::copy_conf() {

   std::filebuf base_file;
   std::filebuf user_file;

   base_file.open(m_base_conf, std::ios::in | std::ios::binary);
   user_file.open(m_user_conf, std::ios::out | std::ios::binary);
   
   std::istreambuf_iterator<char> base_begin(&base_file);
   std::ostreambuf_iterator<char> user_begin(&user_file);

   std::copy(base_begin, {}, user_begin);
}

template<typename T, typename B>
void Config::print_table(std::map<T, B>&  t_table) {

    for ( auto it : t_table){
      std::cout << to_string(it.first) << " " << it.second << std::endl;
   }
}

bool Config::is_valid(int t_result, const char *t_msg, ValidationLog t_log) const{
   
   if (t_result <= 0 && t_log == Error) {
    LOG_ERR("%s", t_msg);
  }

  else if (t_result <= 0 && t_log == Debug) {
    LOG_DEBUG("%s", t_msg);
  }

  else if (t_result <= 0 && t_log == Info) {
    LOG_INFO("%s", t_msg);
  }

  else if (t_result <= 0 && t_log == Critical) {
    LOG_CRIT("%s", t_msg);
  }

  return t_result > 0;
}
