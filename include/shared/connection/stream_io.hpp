#ifndef STREAM_IO_H
#define STREAM_IO_H
#include "IO_strategy.hpp"
#include <netinet/in.h>

class StreamIO: public IOStrategy {

  public:
  using PacketInfoTuple = std::tuple<uint8_t, std::size_t, std::size_t, std::size_t>;

  auto receive(Request &t_req) const -> bool override;
  auto respond(Request &t_req) const -> bool override;

  ~StreamIO() override = default;

  static void send_data(Request &t_req, sockaddr_in t_addrin, Data::DataVector &t_data, PacketInfoTuple &t_pkt_info);
  static void send_header(Request &t_req, sockaddr_in t_addrin, Data::DataVector &t_data);

  static auto receive_data(Request &t_req, sockaddr_in *t_addrin, PacketInfoTuple &t_pkt_info) -> Data::DataVector;
  static auto receive_header(Request &t_req, sockaddr_in *t_addrin) -> PacketInfoTuple;

  static auto make_header(const PacketInfoTuple &t_pkt_info) -> Data::DataVector;
  static auto read_header(Data::DataVector &t_header) -> PacketInfoTuple;

  static auto make_packet_info(Request &t_req, Data::DataVector &t_data) -> PacketInfoTuple;
  static void push_to_header(Data::DataVector &t_header, std::size_t m_header_value);
  static auto read_header_item(Data::DataVector &t_header, std::size_t t_pos) -> std::size_t;

  static void log_packet_info(PacketInfoTuple &t_pkt_info, const char *t_call);

  private:

  /***
   * PACKET Structure
                         HEADER(25 bytes)                      DATA = ~9Kb max
       1 byte     8 bytes     8 bytes        8 bytes
    [[data_type][nb_packet][packet_size][rem_packet_size]]|| [data][data][data] .....

    ***/

  inline static const std::size_t m_NB_SIZE_HEADERS          = 3;
  inline static const std::size_t m_MIN_NB_PACKETS           = 10;
  inline static const std::size_t m_MAX_PACKET_SIZE          = 9000;
  inline static const std::size_t m_SINGLE_PACKAGE_THRESHOLD = 1000;
 

  inline static const std::size_t m_HEADER_SIZE  = sizeof(uint8_t) + (m_NB_SIZE_HEADERS * sizeof(std::size_t)) ;

  /* If this limit is breached m_MAX_NB_PACKETS must grow */


};

#endif // !UDP_TEXT_IO_H
