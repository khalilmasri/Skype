#ifndef CONNECTION_H
#define CONNECTION_H
#include <netdb.h>
#include <netinet/in.h> /* sockeaddr_in */
#include <string>

class Connection {

  public:
  enum ValidationLog { Info, Error, Debug, Critical };

  Connection() : m_port(-1), m_protocol(-1){};
  explicit Connection(int t_port, int t_protocol = SOCK_STREAM) // defaults TCP
      : m_port(t_port), m_protocol(t_protocol){};

  bool         setup(const std::string &_address);
  bool         setup();
  int          get_socket() const;
  sockaddr_in  get_address() const;
  int          get_port() const;
  bool         is_valid(int t_result, const char *t_msg, ValidationLog t_log = Error) const;
  bool         is_setup() const;
  bool         reset_socket();

  static std::string  address_tostring(sockaddr_in t_address);
  static std::string  port_tostring(sockaddr_in t_address);
  static sockaddr_in  to_sockaddr_in(const std::string &t_port, const std::string &t_address);

private:
  int          m_port;
  int          m_socket;
  int          m_protocol; // TCP / UDP
  bool         m_is_setup = false;
  sockaddr_in  m_address; 

  bool         create_socket();
  bool         set_options();
  bool         set_address(const std::string &_address);
  bool         find_address_info();
};

#endif //
