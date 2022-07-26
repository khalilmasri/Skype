#ifndef CONNECTION_POLL_H
#define CONNECTION_POLL_H
#include <poll.h>
#include <iostream>

// TODO: Macros to global config object.
#define MAX_CONNECTIONS 10 
#define ZERO 1 // poll is not zero index.
               
/* m_poll[0] is the local socket for accept_awaits.
 * And m_poll[n] where n > 0 are the connected sockets 
 * We then set an offset of 1 for the 'for' loops.
 */
#define OFFSET 1 

class ConnectionPoll {

  const std::size_t  m_max = MAX_CONNECTIONS;
  struct pollfd      m_poll[MAX_CONNECTIONS];
  int                m_timeout;
  std::size_t        m_poll_index = 0;

  public:
  ConnectionPoll(int t_timeout = 100);

  bool  add_socket(int t_socket_fd);
  void  remove_socket(int t_socket_fd);
  bool  accept_awaits();
  int   select_socket_with_events();
  void  close_all();

  void  reposition_poll();
  void  count_poll_index();

  void  set_timeout(int t_timeout);
  bool  has_timeout(const char t_buffer[]);

};

#endif // !CONNECTION_POOL_H
