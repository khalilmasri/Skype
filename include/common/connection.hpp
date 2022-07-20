#ifndef CONNECTION_H
#define CONNECTION_H
#include <netinet/in.h> /* sockeaddr_in */
#include <string>

class Connection {
 
   int         m_port;
   int         m_socket;
   sockaddr_in m_address;

public:
  Connection(int _port);
  ~Connection();

  bool         setup(const std::string &_address);
  int          get_socket() const;
  sockaddr_in  get_address() const;
  bool         is_valid(int _result, const char* _msg) const;

private:
  bool         create_socket();
  bool         set_options();
  bool         set_address(const std::string &_address);

};

#endif //
