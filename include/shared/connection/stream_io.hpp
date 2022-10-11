#ifndef STREAM_IO_H
#define STREAM_IO_H
#include "IO_strategy.hpp"
#include <netinet/in.h>

class StreamIO: public IOStrategy {
  public:
  auto receive(Request &t_req) const -> bool override;
  auto respond(Request &t_req) const -> bool override;

  ~StreamIO() override = default;

  private:

  static auto send_data(Request &t_req, sockaddr_in t_addrin, Data::DataVector &t_data) -> ssize_t;
  static auto receive_data(Request &t_req, sockaddr_in *t_addrin, Data::DataVector &t_data) -> ssize_t;

  static auto make_header(Request &t_req) -> Data::DataVector;
  static auto read_header(Data::DataVector &t_header) -> std::tuple<Data::type, std::size_t>;


  // Size of the header should be enoug bytes to  to store  size_t + 1 byte for Data::type
  inline static const std::size_t m_HEADER_SIZE = sizeof(std::size_t) + sizeof(uint8_t);
};

#endif // !UDP_TEXT_IO_H
