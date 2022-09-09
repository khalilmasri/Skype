#ifndef DATA_IO_H
#define DATA_IO_H
#include "IO_strategy.hpp"

class DataIO: public IOStrategy {
  public:
  bool         receive(Request &t_req) const override;
  bool         respond(Request &t_req) const override;

  private:

  ~DataIO(){};
  inline static const int m_BUFFER_SIZE = 256;
};

#endif // !DATA_IO_H
