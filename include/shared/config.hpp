#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <filesystem>
#include <unordered_map>

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
    /* cannot copy */
    Config(Config &lhs) = delete;
    void operator = (Config &lhs) = delete;

    // return an imutable reference
    const std::string get(const std::string &t_key) const ;
    const std::string get(const char *t_key) const;

    static Config *get_instance();
    static void   free_instance();

  private:
    static Config    *m_instance;
    const fs::path   m_PATH {std::string(CONFIG_PATH) + getenv("USER") + std::string(CONFIG_FILE) };
    ConfigMap        m_config;

    Config();

    void            load_defaults();
    std::string     read_config_file() const;
    void            load_config_contents(const std::string &t_contents);

    bool            key_exists(const std::string &t_key) const;

};


#endif
