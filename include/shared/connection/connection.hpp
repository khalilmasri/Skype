#ifndef CONNECTION_H
#define CONNECTION_H
#include <netdb.h>
#include <netinet/in.h> /* sockeaddr_in */
#include <string>

class Connection {

  public:
  enum ValidationLog { Info, Error, Debug, Trace, Critical };

  Connection() : m_port(-1), m_protocol(-1){};
  explicit Connection(int t_port, int t_protocol = SOCK_STREAM) // defaults TCP
      : m_port(t_port), m_protocol(t_protocol){};

  auto               setup(const std::string &_address, int timeout = 0) -> bool;
  auto               setup() -> bool;

  [[nodiscard]] auto get_socket() const -> int;
  [[nodiscard]] auto get_address() const -> sockaddr_in;
  [[nodiscard]] auto get_port() const -> int;
  [[nodiscard]] auto is_setup() const -> bool;

  static auto        is_valid(int t_result, const char *t_msg, ValidationLog t_log = Error) -> bool;
  auto               reset_socket() -> bool;

  static auto        address_tostring(sockaddr_in t_address) -> std::string;
  static auto        port_tostring(sockaddr_in t_address) -> std::string;
  static auto        to_sockaddr_in(const std::string &t_port, const std::string &t_address) -> sockaddr_in;

private:
  int                m_port;
  int                m_socket;
  int                m_protocol; // TCP / UDP
  bool               m_is_setup = false;
  sockaddr_in        m_address; 

  auto               create_socket() -> bool;
  [[nodiscard]] auto set_options(int timeout) const -> bool;
  auto               set_address(const std::string &_address) -> bool;
  auto               find_address_info() -> bool;
};

#endif //
