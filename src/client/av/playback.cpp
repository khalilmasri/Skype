#include "playback.hpp"

void Playback::buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages) {
  LOG_INFO("Buffering '%d' packets of Audio data...", nb_packages);
  for (std::size_t count = 0; count < nb_packages; count++) {
    read_package(t_p2p_conn);
  }
}

/* */

void Playback::read_package(P2PPtr &t_p2pconn) {
  Request req = t_p2pconn->make_request();

  t_p2pconn->receive_package(req);

  while (req.data_type() == Data::Empty) {
    t_p2pconn->receive_package(req);
    LOG_TRACE("Received an empty Data Package. Trying again....");
  }

  if (req.data_type() == Data::Done) {
    stop();
    m_done_received = true;
    return;
  }

  const Data *data = req.data();
  load(data); // will load either audio or video depending on inheritance
}

/* */

void Playback::spawn_network_read_thread(P2PPtr &t_p2p_conn, AVStream &t_stream) {

  std::thread playback_thread([this, &t_p2p_conn, &t_stream]() {
    while (m_status == Started) {
      read_package(t_p2p_conn);
    }

    // when a done msg is received we need to stop the AVStream object from sending data to peer
    if (m_done_received) {
      t_stream.stop();
    }
  });

  playback_thread.detach();
}

/* */

auto Playback::valid_data_type(const Data *t_data, Data::Type t_type) -> bool {

  if (t_data->get_type() != t_type) {
    std::string data_type = Data::type_to_string(t_data->get_type());
    std::string desired = Data::type_to_string(t_type);
    LOG_ERR(
        "Attempted to load Playback with the wrong that data type. Was: '%s' "
        "Should be:  '%s'.",
        data_type.c_str(), desired.c_str());
    m_status = Invalid;
    return false;
  }

  return true;
}