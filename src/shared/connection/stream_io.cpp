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

  LOG_TRACE("Sending data to %s", t_req.m_address.c_str());


  if (t_req.data()->get_type() == Data::Text) {
    LOG_ERR("Attempted to send Data::Text in StreamIO. Only Data::Audio and "
            "Data::Video allowed.")
    return false;
  }

  auto [ip, port] = StringUtils::split_first(t_req.m_address, ":");

  Data::DataVector data    = t_req.data()->get_data();
  PacketInfoTuple pkt_info = make_packet_info(t_req, data);
  Data::DataVector header  = make_header(pkt_info); // based on tuy-ple
  sockaddr_in addr_in      = Connection::to_sockaddr_in(port, ip);

  LOG_TRACE("Sending data packet size %lu", data.size());

  if (header.size() != m_HEADER_SIZE) {
    LOG_ERR("Header size is incorrect. Was: %d , should be: %d", header.size(), m_HEADER_SIZE);
    t_req.m_valid = false;
  }

  /* send header first */
  if (t_req.m_valid) {
    send_header(t_req, addr_in, header);
  }

  /* send data header was sent succesfully */
  if (t_req.m_valid) {
    send_data(t_req, addr_in, data, pkt_info);
  }

  log_packet_info(pkt_info, "sent");

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

  /* peer sent an empty data stream with a done message. */
  if(data_type == Data::Done){
    t_req.set_data(new AVData(Data::Done));
    return t_req.m_valid;
  }

  Data::DataVector data = receive_data(t_req, &addr_in, pkt_info);
  t_req.m_address       = Connection::address_tostring(addr_in) + ":" + Connection::port_tostring(addr_in);

  if (t_req.m_valid) { 
    LOG_TRACE("Received total data: %llu", data.size());
    t_req.set_data(new AVData(std::move(data), data_type));
  } 


  log_packet_info(pkt_info, "received");
  
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
  
  // tuple extract
  auto [data_type, nb_packets, packet_size, rem_packet_size] = t_pkt_info;
  uint8_t *data = t_data.data(); // vector

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

  if(rem_packet_size == 0){
      return;
  }

  /* send remainder packet */
 int result = sendto(t_req.m_socket, data, rem_packet_size, 0, reinterpret_cast<struct sockaddr *>(&t_addrin), sizeof(t_addrin));

  if(result < 0){
      LOG_ERR("Could not send remainder packet type '%c' of size '%d'", data_type, packet_size);
      t_req.m_valid = false;
    }

}


auto StreamIO::receive_header(Request &t_req, sockaddr_in *t_addrin) -> PacketInfoTuple{

  socklen_t addr_len = sizeof((*t_addrin));
  Data::DataVector header(m_HEADER_SIZE, 0);

   int result = recvfrom(t_req.m_socket, header.data(), m_HEADER_SIZE, 0, reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);

   if(result < 0){
      LOG_ERR("StreamIO could not read header.");
      t_req.m_valid = false;
      return {0,0,0,0};
   }

   while(result != m_HEADER_SIZE){
      result = recvfrom(t_req.m_socket, header.data(), m_HEADER_SIZE, 0, reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);
   }

   return read_header(header);
}


/* */

auto StreamIO::receive_data(Request &t_req, sockaddr_in *t_addrin, PacketInfoTuple &t_pkt_info) -> Data::DataVector {

  socklen_t addr_len                                  = sizeof((*t_addrin));
  auto [data_type, nb_packets, packet_size, rem_size] = t_pkt_info;
  std::size_t alloc_size                              = (nb_packets * packet_size) + rem_size;

 if( alloc_size >= 1000000){
   LOG_ERR("Allocating exceeds the 1000000 bytes. Returning Empty vector.");
   return {};
  }
 
  Data::DataVector data(alloc_size, 0);

  uint8_t *data_pointer = data.data();


  for (std::size_t packet_index = 0; packet_index < nb_packets; packet_index++) {
      int result = recvfrom(t_req.m_socket, data_pointer, packet_size, 0, reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);

      data_pointer += packet_size;

      if(result < 0){
        LOG_ERR("Could not receive packet type '%c' number '%d' out of '%d' of size '%d'",data_type, packet_index, nb_packets, packet_size);
        t_req.m_valid = false;
      }
    }

  /* Remainder: (don't receive if 0) */
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
  uint8_t data_type      = static_cast<Data::Type>( t_header[header_pos]); 
  header_pos++;


  /* Grab the size headers */
  std::array<std::size_t, m_NB_SIZE_HEADERS> size_headers{0}; // init to zero prevent garbage

  for(std::size_t i = 0; i < m_NB_SIZE_HEADERS; i++ ){
    size_headers[i] = read_header_item(t_header, header_pos);
    header_pos += sizeof(std::size_t);
  }

  return {data_type, size_headers[0], size_headers[1], size_headers[2]};
}

/* */

auto StreamIO::make_packet_info(Request &t_req, Data::DataVector &t_data) -> PacketInfoTuple {

  /*
   * NOTE: Example -> t_data.size()             = 83116
           83116 % 10                           = 6 (remainder packets size)
           floor(83116) / 10[m_MAX_NB_PACKETS]) = 8311 (packets size)
           floor(83116 / 8311)                  = 10 (number of packets)
           return tuple with  {'v', 10, 8311, 6}     -> there is always  1 remainder
                                                  so we don't count as nb_packets for it.
  */

  uint8_t data_type = Data::type_to_char(t_req.data()->get_type());
  std::size_t grow_by = 0;
   std::size_t max_total_data = m_MIN_NB_PACKETS * m_MAX_PACKET_SIZE;

   /* return just a single package with no remainder */
  if (t_data.size() < m_SINGLE_PACKAGE_THRESHOLD) {
    return {data_type, 1, t_data.size(), 0}; 
  };

  /* grow the nb_packets dynamically when t_data.size() is too big for ~9K packets (m_MAX_PACKET_SIZE) */
  if(t_data.size() > max_total_data){
    grow_by = (t_data.size() - max_total_data) / m_MAX_PACKET_SIZE + 1;
  }

  std::size_t rem_packet_size = t_data.size() % (m_MIN_NB_PACKETS + grow_by);
  std::size_t packet_size     = std::floor(t_data.size() / (m_MIN_NB_PACKETS + grow_by)); 
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
    size_in_bytes[i] = t_header[t_pos + i];
  }

  uint8_t *data_in_bytes = size_in_bytes.data();
  std::size_t *data      = reinterpret_cast<std::size_t *>(data_in_bytes);
  return *data;
}

void StreamIO::log_packet_info(PacketInfoTuple &t_pkt_info, const char *t_call){
   auto [type, nb_packets, size_packet, rem_size] = t_pkt_info;

   Data::Type data_type = Data::char_to_type(type);
   LOG_TRACE("Package %s. Type: '%s', NB Packets: '%d ', Packet Size: '%d';, Remainder Size: '%d' ",
       t_call, Data::type_to_string(data_type).c_str(), nb_packets, size_packet, rem_size);
}

/* TESTS */

TEST_CASE("Stream IO") {
  auto io = StreamIO();

  std::size_t size =  84238;
  Data::DataVector dvector(size);
  AVData *data = new AVData(std::move(dvector), Data::Audio);

  auto req = Request(true);
  req.set_data(data);

  auto pkt_info= StreamIO::make_packet_info(req, dvector);

  SUBCASE("Make Header (normal size)") {
    auto nb_packets = std::get<1>(pkt_info);
    auto packet_size = std::get<2>(pkt_info);
    auto rem_size = std::get<3>(pkt_info);

    std::size_t total_size = packet_size * nb_packets + rem_size;

    CHECK(std::get<0>(pkt_info) == 'a'); // type
    CHECK(nb_packets == 10); 
    CHECK(packet_size == 8423); 
    CHECK(rem_size == 8); 
    CHECK(total_size == size); 
  }

  SUBCASE("read headers(normal size)") {
  Data::DataVector header = io.make_header(pkt_info);
  auto pkt_info_out = io.read_header(header);

  CHECK(std::get<0>(pkt_info_out) == std::get<0>(pkt_info));
  CHECK(std::get<1>(pkt_info_out) == std::get<1>(pkt_info));
  CHECK(std::get<2>(pkt_info_out) == std::get<2>(pkt_info));
  CHECK(std::get<3>(pkt_info_out) == std::get<3>(pkt_info));
  }

  size = 98477;
  Data::DataVector dvec2(size);
  req.set_data(new AVData(std::move(dvec2), Data::Video));
  pkt_info= StreamIO::make_packet_info(req, dvec2);

  SUBCASE("Make Header (force to grow)") {
    auto nb_packets = std::get<1>(pkt_info);
    auto packet_size = std::get<2>(pkt_info);
    auto rem_size = std::get<3>(pkt_info);

    std::size_t total_size = packet_size * nb_packets + rem_size;

    CHECK(std::get<0>(pkt_info) == 'v'); // type
    CHECK(nb_packets == 11); 
    CHECK(packet_size == 8952); 
    CHECK(rem_size == 5); 
    CHECK(total_size == size); 
  }


  SUBCASE("read headers(force to grow)") {
   Data::DataVector header = io.make_header(pkt_info);
  auto pkt_info_out = io.read_header(header);

  CHECK(std::get<0>(pkt_info_out) == std::get<0>(pkt_info));
  CHECK(std::get<1>(pkt_info_out) == std::get<1>(pkt_info));
  CHECK(std::get<2>(pkt_info_out) == std::get<2>(pkt_info));
  CHECK(std::get<3>(pkt_info_out) == std::get<3>(pkt_info));
  }

  size = 198477;
  Data::DataVector dvec3(size);
  req.set_data(new AVData(std::move(dvec3), Data::Empty));
  pkt_info= StreamIO::make_packet_info(req, dvec3);

  SUBCASE("Make Header (huge size)") {
    auto nb_packets = std::get<1>(pkt_info);
    auto packet_size = std::get<2>(pkt_info);
    auto rem_size = std::get<3>(pkt_info);

    std::size_t total_size = packet_size * nb_packets + rem_size;

    CHECK(std::get<0>(pkt_info) == 'e'); // type
    CHECK(nb_packets == 23); 
    CHECK(packet_size == 8629); 
    CHECK(rem_size == 10); 
    CHECK(total_size == size); 
  }


  size = 900;
  Data::DataVector dvec4(size);
  req.set_data(new AVData(std::move(dvec4), Data::Audio));
  pkt_info= StreamIO::make_packet_info(req, dvec4);

  SUBCASE("Make Header (single)") {
    auto nb_packets = std::get<1>(pkt_info);
    auto packet_size = std::get<2>(pkt_info);
    auto rem_size = std::get<3>(pkt_info);

    std::size_t total_size = packet_size * nb_packets + rem_size;

    CHECK(std::get<0>(pkt_info) == 'a'); // type
    CHECK(nb_packets == 1); 
    CHECK(packet_size == 900); 
    CHECK(rem_size == 0); 
    CHECK(total_size == size); 
  }


 size = 4000;
  Data::DataVector dvec5(size);
  req.set_data(new AVData(std::move(dvec5), Data::Audio));
  pkt_info= StreamIO::make_packet_info(req, dvec5);

  SUBCASE("Make Header (single)") {
    auto nb_packets = std::get<1>(pkt_info);
    auto packet_size = std::get<2>(pkt_info);
    auto rem_size = std::get<3>(pkt_info);

    std::size_t total_size = packet_size * nb_packets + rem_size;

    CHECK(std::get<0>(pkt_info) == 'a'); // type
    CHECK(nb_packets == 10); 
    CHECK(packet_size == 400); 
    CHECK(rem_size == 0); 
    CHECK(total_size == size); 
  }
}
