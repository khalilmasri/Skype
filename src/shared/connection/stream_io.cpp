#include "stream_io.hpp"
#include "av_data.hpp"
#include "connection.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "string_utils.hpp"
#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <netinet/in.h>
#include <typeinfo>
#include <unistd.h>

auto StreamIO::respond(Request &t_req) const -> bool {

  LOG_DEBUG("Sending data to %s", t_req.m_address.c_str());

  if (t_req.data()->get_type() == Data::Text) {
    LOG_ERR("Attempted to send Data::Text in StreamIO. Only Data::Audio and "
            "Data::Video allowed.")
    return false;
  }

  auto [ip, port] = StringUtils::split_first(t_req.m_address, ":");

  Data::DataVector data    = t_req.data()->get_data();
  PacketInfoTuple pkt_info = get_packet_info(t_req, data);
  Data::DataVector header  = make_header(pkt_info);
  sockaddr_in addr_in      = Connection::to_sockaddr_in(port, ip);

  if (header.size() != m_HEADER_SIZE) {
    LOG_ERR("Header size is incorrect. Was: %d , should be: %d", header.size(),
            m_HEADER_SIZE);
    t_req.m_valid = false;
  }


  /* send header first */
  if (t_req.m_valid) {
    send_data(t_req, addr_in, header, pkt_info);
  }

  LOG_DEBUG("Sent header! %d", header.size());

  /* send data header was sent succesfully */
  if (t_req.m_valid) {
    send_data(t_req, addr_in, data, pkt_info);
  }

  return t_req.m_valid;
};

/* */

auto StreamIO::receive(Request &t_req) const -> bool {

  sockaddr_in addr_in;
  PacketInfoTuple pkt_info = receive_header(t_req, &addr_in);

  Data::Type data_type = Data::char_to_type(std::get<0>(pkt_info)); // grab type here

  if(data_type == Data::Empty){
  t_req.set_data(new AVData());
  return t_req.m_valid;
  }

  Data::DataVector data = receive_data(t_req, &addr_in, pkt_info);
  t_req.m_address       = Connection::address_tostring(addr_in) + ":" + Connection::port_tostring(addr_in);

  if (t_req.m_valid) { 
    t_req.set_data(new AVData(std::move(data), data_type));
  } 

  return t_req.m_valid;
};


/* */

void StreamIO::send_header(Request &t_req, sockaddr_in t_addrin, Data::DataVector &t_header) {

  int result = sendto(t_req.m_socket, t_header.data() , t_header.size(), 0, reinterpret_cast<struct sockaddr *>(&t_addrin), sizeof(t_addrin));

    if(result < 0){
      auto [data_type, nb_packets, packet_size, rem_size] = read_header(t_header);
      LOG_ERR("Could not send header: data_type: '%c', nb_packets: %d, packet_size: %d, rem_packet_size: %d",
          data_type, nb_packets, packet_size, rem_size);

      t_req.m_valid = false;
    }
}


/* */

void StreamIO::send_data(Request &t_req, sockaddr_in t_addrin, Data::DataVector &t_data, PacketInfoTuple &t_pkt_info) {
  
  auto [data_type, nb_packets, packet_size, rem_packet_size] = t_pkt_info;
  uint8_t *data = t_data.data();

  /* send first packets */
  for (std::size_t packet_index = 0; packet_index < nb_packets; packet_index++) {
    int result = sendto(t_req.m_socket, data, packet_size, 0, reinterpret_cast<struct sockaddr *>(&t_addrin), sizeof(t_addrin));
    
    /* increment pointer to get the next set of data */
    data    += packet_size;

    if(result < 0){
      LOG_ERR("Could not send packet type '%c' number '%d' out of '%d' of size '%d'",data_type, packet_index, nb_packets, packet_size);
      t_req.m_valid = false;
    }
  }

  /* send remainder packet */
 int result = sendto(t_req.m_socket,data, rem_packet_size, 0, reinterpret_cast<struct sockaddr *>(&t_addrin), sizeof(t_addrin));

  if(result < 0){
      LOG_ERR("Could not send remainder packet type '%c' of size '%d'", data_type, packet_size);
      t_req.m_valid = false;
    }

}


auto StreamIO::receive_header(Request &t_req, sockaddr_in *t_addrin) -> PacketInfoTuple{

  socklen_t addr_len = sizeof((*t_addrin));
  Data::DataVector header;

  header.reserve(m_HEADER_SIZE);

   int result = recvfrom(t_req.m_socket, header.data(), m_HEADER_SIZE, 0,
                  reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);

   if(result < 0){
      LOG_ERR("StreamIO could not read header.");
      t_req.m_valid = false;
      return {0,0,0,0};
   }

   return read_header(header);
}


/* */

auto StreamIO::receive_data(Request &t_req, sockaddr_in *t_addrin, PacketInfoTuple &t_pkt_info) -> Data::DataVector {

  socklen_t addr_len = sizeof((*t_addrin));
  auto [data_type, nb_packets, packet_size, rem_size] = t_pkt_info;

  Data::DataVector data;
  data.reserve((nb_packets * packet_size) + rem_size);
  uint8_t *data_pointer = data.data();


  for (std::size_t packet_index = 0; packet_index < nb_packets; packet_index++) {
      int result    = recvfrom(t_req.m_socket, data_pointer, packet_size, 0, reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);
      data_pointer += packet_size;

      if(result < 0){
        LOG_ERR("Could not receive packet type '%c' number '%d' out of '%d' of size '%d'",data_type, packet_index, nb_packets, packet_size);
        t_req.m_valid = false;
      }
    }

  /*Remainder: (don't receive if 0) */
   if(rem_size == 0){
     return data;
   }

    int result = recvfrom(t_req.m_socket, data_pointer, rem_size, 0, reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);
   
    if(result < 0){
      LOG_ERR("Could not receive remainder type '%c'of size '%d'",data_type, rem_size);
      t_req.m_valid = false;
    }

  return data;
}

/* */

auto StreamIO::make_header(const PacketInfoTuple &t_pkt_info) -> Data::DataVector {

  auto [data_type, nb_packets, packet_size, rem_packet_size] = t_pkt_info;

  auto header = Data::DataVector();

  header.push_back(data_type);
  push_to_header(header, nb_packets);
  push_to_header(header, packet_size);
  push_to_header(header, rem_packet_size);

  return header;
}

/* */

auto StreamIO::read_header(Data::DataVector &t_header) -> PacketInfoTuple {

  /* Grab data_type */
  std::size_t header_pos = 0;
  uint8_t data_type      = static_cast<Data::Type>( t_header.at(header_pos)); 
  header_pos++;


  /* Grab the size headers */
  std::array<std::size_t, m_NB_SIZE_HEADERS> size_headers;

  for(std::size_t i = 0; i < m_NB_SIZE_HEADERS; i++ ){
    size_headers[i] = read_header_item(t_header, header_pos);
    header_pos += sizeof(std::size_t);
  }

  return {data_type, size_headers.at(0), size_headers.at(1), size_headers.at(2)};
}

/* */

auto StreamIO::get_packet_info(Request &t_req, Data::DataVector &t_data) -> PacketInfoTuple {

  /*
   * NOTE: t_data.size() = 83116
     83116 % 10         = 6 (remainder packets size)
     floor(83116) / 10  = 8311 (packets size)
     floor(83116 / 8311) = 10 (number of packets)
     return tuple with {10, 8311, 6} -> there is always a remainder so we don't count as nb_packets
  */

  uint8_t data_type = Data::type_to_char(t_req.data()->get_type());

  if (t_data.size() < m_SINGLE_PACKAGE_THRESHOLD) {
    return {data_type, 1, t_data.size(), 0}; // return just a single package with no remainder
  };

  std::size_t rem_packet_size = t_data.size() % m_MAX_NB_PACKETS;
  std::size_t packet_size     = std::floor(t_data.size() / m_MAX_NB_PACKETS); // floor if decimal point
  std::size_t nb_packets      = std::floor(t_data.size() / packet_size);

  return {data_type, nb_packets, packet_size, rem_packet_size};
};

/* */

void StreamIO::push_to_header(Data::DataVector &t_header, std::size_t m_header_value) {

  auto *value_in_bytes = reinterpret_cast<uint8_t *>(&m_header_value);

  for (std::size_t i = 0; i < sizeof(std::size_t);
       i++) { // get bytes from std::size_t
    t_header.push_back(value_in_bytes[i]);
  }
}


auto StreamIO::read_header_item(Data::DataVector &t_header, std::size_t t_pos) -> std::size_t{

  std::array<uint8_t, sizeof(std::size_t)> size_in_bytes;
  for (std::size_t i = 0; i < size_in_bytes.size(); i++) {
    size_in_bytes[i] = t_header.at(t_pos + i);
  }

  uint8_t *data_in_bytes = size_in_bytes.data();
  std::size_t *data      = reinterpret_cast<std::size_t *>(data_in_bytes);
  return *data;
}

/* TESTS */

TEST_CASE("Stream IO") {

  auto io = StreamIO();

  Data::DataVector dvector(10);
  AVData *data = new AVData(std::move(dvector), Data::Audio);

  auto req = Request(true);
  req.set_data(data);

  Data::DataVector r = io.make_header(req);

  std::cout << "result size : " << r.size() << "\n";
  std::cout << "result 1 item : " << r.at(0) << "\n";
}
