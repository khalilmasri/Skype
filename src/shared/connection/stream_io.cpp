#include "stream_io.hpp"
#include "av_data.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "string_utils.hpp"
#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <typeinfo>
#include <unistd.h>

auto StreamIO::respond(Request &t_req) const -> bool {

  if (t_req.data()->get_type() == Data::Text) {
    LOG_ERR("Attempted to send Data::Text in StreamIO. Only Data::Audio and "
            "Data::Video allowed.")
    return false;
  }

  auto [ip, port] = StringUtils::split_first(t_req.m_address, ":");
  Data::DataVector data = t_req.data()->get_data();
  Data::DataVector header = make_header(t_req);
  sockaddr_in addr_in = Connection::to_sockaddr_in(port, ip);

  if (header.size() != m_HEADER_SIZE) {
    LOG_ERR("Header size is incorrect. Was: %d , should be: %d", header.size(),
            m_HEADER_SIZE);
    t_req.m_valid = false;
  }

  ssize_t res = 0;

  /* send header first */
  if (t_req.m_valid) {
    res = send_data(t_req, addr_in, header);
    t_req.m_valid =
        is_valid(static_cast<int>(res), "UDPTextIO could not send header.");
  }

  /* send data header was sent succesfully */
  if (t_req.m_valid) {
    res = send_data(t_req, addr_in, data);
    t_req.m_valid =
        is_valid(static_cast<int>(res), "UDPTextIO could not send data.");
  }

  return t_req.m_valid;
};

auto StreamIO::receive(Request &t_req) const -> bool {

  Data::DataVector header(m_HEADER_SIZE);
  sockaddr_in addr_in;

  /* Check header */
  if (header.size() != m_HEADER_SIZE) {
    LOG_ERR("Header size is incorrect. Was: %d , should be: %d", header.size(), m_HEADER_SIZE);
    t_req.m_valid = false;
  }

  /* header */
  ssize_t res = receive_data(t_req, &addr_in, header);
  t_req.m_valid = is_valid(static_cast<int>(res), "StreamIO could not receive header.");
  auto [data_type, data_size] = read_header(header);

  /* Data */
  Data::DataVector data(
      data_size); // allocate with the size that needs to be read.
  res = receive_data(t_req, &addr_in, data);
  t_req.m_valid = is_valid(static_cast<int>(res), "StreamIO could not receive data.");

  /* set response */
  if (t_req.m_valid) {
    t_req.m_address = Connection::address_tostring(addr_in) + ":" + Connection::port_tostring(addr_in);
    t_req.set_data(new AVData(std::move(data), data_type));
  }

  return t_req.m_valid;
};

auto StreamIO::send_data(Request &t_req, sockaddr_in t_addrin,
                         Data::DataVector &t_data) -> ssize_t {

  return sendto(t_req.m_socket, t_data.data(), t_data.size(), 0,
                reinterpret_cast<struct sockaddr *>(&t_addrin),
                sizeof(t_addrin));
}

auto StreamIO::receive_data(Request &t_req, sockaddr_in *t_addrin,
                            Data::DataVector &t_data) -> ssize_t {

  socklen_t addr_len = sizeof((*t_addrin));

  return recvfrom(t_req.m_socket, t_data.data(), t_data.size(), 0,
                  reinterpret_cast<struct sockaddr *>(t_addrin), &addr_len);
}

auto StreamIO::make_header(Request &t_req) -> Data::DataVector {

  auto header = Data::DataVector();

  // push data type to header
  header.push_back(static_cast<uint8_t>(t_req.data()->get_type()));

  // push size to header
  std::size_t size = t_req.data()->size();
  auto *size_in_bytes = reinterpret_cast<uint8_t *>(size);

  for (std::size_t i = 0; i < sizeof(std::size_t); i++) { // get bytes from int
    header.push_back(size_in_bytes[i]);
  }

  return header;
}

auto StreamIO::read_header(Data::DataVector &t_header)
    -> std::tuple<Data::type, std::size_t> {

  auto type = static_cast<Data::type>(t_header.at(0));

  std::array<uint8_t, sizeof(std::size_t)> size_in_bytes;

  for (std::size_t i = 0; i < sizeof(std::size_t); i++) {
    // + 1 to skip type which is the first element
    size_in_bytes[i] = t_header.at(i + 1);
  }
  auto size = reinterpret_cast<std::size_t>(size_in_bytes.data());

  return {type, size};
}
