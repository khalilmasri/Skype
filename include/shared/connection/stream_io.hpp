#ifndef STREAM_IO_H
#define STREAM_IO_H
#include "IO_strategy.hpp"

class StreamIO: public IOStrategy {
  public:
  auto receive(Request &t_req) const -> bool override;
  auto respond(Request &t_req) const -> bool override;

  private:

  ~StreamIO() override = default;
  inline static const int m_BUFFER_SIZE = 512;
};

#endif // !UDP_TEXT_IO_H
