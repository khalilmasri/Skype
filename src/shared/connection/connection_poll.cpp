#include "connection_poll.hpp"
//#include "connection.hpp"
#include "logger.hpp"

#include <unistd.h>

ConnectionPoll::ConnectionPoll(int t_timeout) : m_timeout(t_timeout) {

  for (std::size_t i = m_poll_index; i < m_max;
       i++) { /* initialize fds to -1 */
    m_poll[i].fd = -1;
  }
}

bool ConnectionPoll::add_socket(int t_socket_fd) {

  /* add connected sockets to polling system */

  if (m_poll_index >= m_max) {
    return false;
  }

  m_poll[m_poll_index].fd = t_socket_fd;
  m_poll[m_poll_index].events = 0;
  m_poll[m_poll_index].events |= POLLIN;
  m_poll_index++;

  return true;
}

bool ConnectionPoll::accept_awaits() {

  int res = poll(m_poll, ZERO, m_timeout);

  return res > 0 ? true : false;
}

int ConnectionPoll::select_socket_with_events() {
  for (std::size_t i = OFFSET; i < m_max; i++) {

    int res = poll(m_poll, i + ZERO, m_timeout);

    if (res > 0) {

      if (m_poll[i].fd == -1) {
        continue;
      }

      if (m_poll[i].revents & POLLIN) {
        return m_poll[i].fd;
      }
    }
  }
  return -1;
}

void ConnectionPoll::remove_socket(int t_socket_fd) {
  for (std::size_t i = OFFSET; i < m_max; i++) {
    if (m_poll[i].fd == t_socket_fd) {
      close(m_poll[i].fd);
      m_poll[i].fd = -1;
    }
  }

  reposition_poll();  // reposition if removing from the middle
  count_poll_index(); // recount m_poll_index
}

void ConnectionPoll::reposition_poll() {

  for (std::size_t i = OFFSET; i < m_max; i++) {

    if (i == m_max - 1) { // break on last one
      break;
    }

    if (m_poll[i].fd == -1 &&
        m_poll[i + 1].fd != -1) { // if current fd is invalid and next is valid
      m_poll[i].fd = m_poll[i + 1].fd; // move existing fd rightwards
      m_poll[i + 1].fd = -1;
    }
  }
}

void ConnectionPoll::count_poll_index() {
  std::size_t count = OFFSET;

  for (std::size_t i = OFFSET; i < m_max; i++) {

    if (m_poll[i].fd == -1) {
      break;
    }

    count++;
  }
  m_poll_index = count;
}

void ConnectionPoll::close_all() {
  for (std::size_t i = 1; i < m_max; i++) {
    close(m_poll[i].fd);
  }
}

void ConnectionPoll::set_timeout(int t_timeout) { m_timeout = t_timeout; }

bool ConnectionPoll::has_timeout(const char t_type[]) {

   int res = poll(m_poll, ZERO, m_timeout);

  if (res == 0) {
    LOG_INFO("%s timeout.", t_type);
    return true;
  }

  else
    return false;
}
