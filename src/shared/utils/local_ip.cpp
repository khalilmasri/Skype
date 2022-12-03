#include "local_ip.hpp"
#include "logger.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

LocalIP::LocalIP() : m_status(Valid) {

  struct ifaddrs *ifaddr = get_ifaddrs();
  m_status = Valid;

  find_local_ip(ifaddr);
  freeifaddrs(ifaddr);
};

std::string LocalIP::get_first() const {

  if (m_status != Valid) {
    LOG_ERR("LocalIP is invalid. reason: ", m_reason.c_str());
    return {};
  }

  if (!m_ip_addresses.empty()) {
    return m_ip_addresses.at(0);
  } else {

    LOG_ERR("LocalIP does not contain any IP addresses.");
    return {};
  }

}

std::string LocalIP::get_at(std::size_t t_pos) const {

  if (m_status != Valid) {
    LOG_ERR("LocalIP is invalid. reason: ", m_reason.c_str());
    return {};
  }

  try {
    return m_ip_addresses.at(t_pos);

  } catch (...) {
    LOG_ERR("LocalIP: IP Address at position %d does not exist.", t_pos);
    return {};
  }
}

/* Private */

struct ifaddrs *LocalIP::get_ifaddrs() {

  struct ifaddrs *ifaddr;
  int res = getifaddrs(&ifaddr);

  if (res == -1) {
    handle_error(res, "getifaddress()");
  }

  return ifaddr;
}

/* */

void LocalIP::find_local_ip(ifaddrs *t_ifaddr) {

  int res;
  char host[NI_MAXHOST];

  for (struct ifaddrs *ifa = t_ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

    if (ifa->ifa_addr == NULL) {
      continue;
    }

    if (ifa->ifa_addr->sa_family == AF_INET) {

      res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
                        NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

      if (res != 0) {
        handle_error(res, "getnameinfo()");
      }

      if (m_LOCALHOST != host) { // ignores localhost.
        m_ip_addresses.push_back(host);
      }
    }
  }
}

/* */

void LocalIP::handle_error(int t_err, const char *t_msg) {
  const char *err = gai_strerror(t_err);
  m_reason = t_msg;
  m_reason.append(" call was unsuccessful: ");
  m_reason.append(err);
  m_status = Invalid;
}
