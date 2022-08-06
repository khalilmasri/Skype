#ifndef CONNECTION_H
#define CONNECTION_H
#include <netinet/in.h> /* sockeaddr_in */
#include <string>

class Connection {
   int          m_port;
   int          m_socket;
   bool         m_is_setup = false;
   sockaddr_in  m_address;

public:
  enum ValidationLog { Info, Error, Debug, Critical };

  Connection(): m_port(-1){}; 
  explicit Connection(int t_port)
    : m_port(t_port){};

  bool         setup(const std::string &_address);
  int          get_socket() const;
  sockaddr_in  get_address() const;
  int          get_port() const;
  bool         is_valid(int t_result, const char *t_msg, ValidationLog t_log = Error) const;
  bool         is_setup() const;
  std::string  address_tostring(sockaddr_in t_address);

private:
  bool         create_socket();
  bool         set_options();
  bool         set_address(const std::string &_address);
};

#endif //


