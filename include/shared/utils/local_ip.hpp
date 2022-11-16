#ifndef LOCAL_IP_H
#define LOCAL_IP_H
#include <ifaddrs.h>
#include <string>
#include <vector>

class LocalIP {

public:
  enum Status { Valid, Invalid };

  explicit LocalIP();

  std::string get_first() const;
  std::string get_at(std::size_t t_pos) const;

private:
  typedef std::vector<std::string> IPVector;

  const std::string m_LOCALHOST = "127.0.0.1";
  std::string       m_reason;
  Status            m_status = Valid;
  IPVector          m_ip_addresses;

  struct ifaddrs *get_ifaddrs();

  void find_local_ip(ifaddrs *t_ifaddr);
  void handle_error(int t_err, const char *t_msg);
};

#endif
