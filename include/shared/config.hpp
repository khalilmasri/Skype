#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <filesystem>
#include <unordered_map>
#include <filesystem>

#define CONFIG_FILE "/.skype.conf"
#define CONFIG_PATH "/home/"
#define CONFIG_DELIM "="

#if __linux__
namespace fs = std::filesystem;

#elif __APPLE__ || __MACH__
namespace fs = std::__fs::filesystem;

#endif


class Config {

  typedef std::unordered_map<std::string, std::string> ConfigMap;

  public:
    /* no copy for singletons */
    Config(Config &lhs) = delete;
    void operator = (Config &lhs) = delete;

    /* return either a string or integer */
    template<typename T> T get(const char *t_key) const;

    /* constructs the database string */
    const std::string  get_db() const;

    static Config      *get_instance();
    static void        free_instance(); 

  private:

    /**
     
      DEFAULTS

      m_DEFAULTS must have an entry for every key-pair value in skype.conf 
      If it is not here the program WILL NOT read that value from skype.conf

    **/                                  

    inline static const ConfigMap    m_DEFAULTS {       
      {"ENV", "DEV"},                                   
      {"TCP_PORT", "5000"},                             
      {"UDP_PORT", "7000"},                             
      {"SERVER_ADDRESS", "127.0.0.1"},                  
      {"LOCAL_IP", "127.0.0.1"},                        
      {"REMOTE_IP", "206.189.0.154"},                   
      {"HEADER_LENGTH", "10"},                          
      {"DB_USER", "postgres"},                          
      {"DB_PASSWORD", "postgres"},                      
      {"DB_ADDRESS", "localhost"},                      
      {"DB_NAME", "skype"},                             
      {"DB_PORT", "5432"},
      {"LOGGER_LEVEL", "trace"},
      {"DEBUG_ENABLE", "0"}
    };

    /* */                                  

    static Config       *m_instance;


   #if __APPLE__ || __MACH__ || __linux__

   /* path to .conf /home/$USER/.skype.conf for OSX & Linux */                                  
   const fs::path      m_PATH {getenv("HOME") + std::string(CONFIG_FILE) };
   #else

   /* Other systems not supported. use defaults. */                                  
   const fs::path       m_PATH {};

   #endif

    ConfigMap           m_config;

    Config();

    void               load_defaults();
    std::string        read_config_file() const;
    void               load_config_contents(const std::string &t_contents);

    void               set_server_address();

    int                get_int(const char *t_key) const;
    const std::string  get_str(const char *t_key) const;

    bool               key_exists(const std::string &t_key) const;

};

#endif
