#ifndef UDP_TEXT_IO_H
#define UDP_TEXT_IO_H
#include "IO_strategy.hpp"

class UDPTextIO: public IOStrategy {
  public:
  auto receive(Request &t_req) const -> bool override;
  auto respond(Request &t_req) const -> bool override;

  private:

  ~UDPTextIO() override = default;
  inline static const int m_BUFFER_SIZE = 512;
};

#endif // !UDP_TEXT_IO_H
