#ifndef UDP_TEXT_IO_H
#define UDP_TEXT_IO_H
#include "IO_strategy.hpp"

class UDPTextIO: public IOStrategy {
  public:
  bool         receive(Request &t_req) const override;
  bool         respond(Request &t_req) const override;

  private:

  ~UDPTextIO(){};
  inline static const int m_BUFFER_SIZE = 512;
};

#endif // !UDP_TEXT_IO_H
