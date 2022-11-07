#include "audio_playback.hpp"
#include "logger.hpp"

AudioPlayback::AudioPlayback()
    : m_audio_output(m_audio_queue, AudioDevice::Output) {}

void AudioPlayback::start(P2PPtr &t_p2pconn) {

  if (m_status == Stopped) {
    m_audio_output.open(); // Audio
    m_status = Started;

    std::thread playback_thread([this, &t_p2pconn]() {
      while (m_status == Started) {
        read_package(t_p2pconn);
        if (true == m_should_hangup)
        {
          stop();
        }
      }
    });

    playback_thread.detach();

  } else {
    LOG_ERR("Could not START audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}
void AudioPlayback::stop() {

  if (m_status == Started) {

    // wait for the queue to be empty before stopping.
    while (!m_audio_queue->empty()) {
      AudioDevice::wait(1); // wait 1 frame
    }

    m_audio_output.close();
    m_status = Stopped;

  } else {
    LOG_ERR("Could not STOP audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void AudioPlayback::buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages) {
  LOG_INFO("Buffering '%d' packets of Audio/Video data...", nb_packages);
  for (std::size_t count = 0; count < nb_packages; count++) {
    read_package(t_p2p_conn);
  }
};

void AudioPlayback::read_package(P2PPtr &t_p2pconn) {

  Request req = t_p2pconn->make_request();

  t_p2pconn->receive_package(req);

  uint32_t empty_packages_counter = 0;
  while (req.data_type() == Data::Empty) {
    t_p2pconn->receive_package(req);
    if (MAX_EMPTY_PACKAGES == empty_packages_counter)
    {
      m_should_hangup = true;
    }
    empty_packages_counter++;
    LOG_TRACE("Received an empty audio packaged. Trying again....");
  }

  const Data *audio_data = req.data();
  load_audio(audio_data);
}

/* */

void AudioPlayback::load_audio(const Data *t_audio_data) {

  if (valid_data_type(t_audio_data, Data::Audio)) {

    std::vector<uint8_t> audio = t_audio_data->get_data();
    Data::DataVector decoded_audio = m_audio_converter.decode(audio);
    AudioPackage audio_pkt(std::move(decoded_audio));
    m_audio_queue->push(std::move(audio_pkt));

    if (!m_audio_converter.valid()) {
      LOG_ERR("Error converting audio to output.");
    }
  }
}

auto AudioPlayback::valid_data_type(const Data *t_data, Data::Type t_type)
    -> bool {

  if (t_data->get_type() != t_type) {
    std::string data_type = Data::type_to_string(t_data->get_type());
    std::string desired = Data::type_to_string(t_type);
    LOG_ERR("Attempted to load AVPlayback with the wrong that type. Was: '%s' "
            "Should be:  '%s'.",
            data_type.c_str(), desired.c_str());
    m_status = Invalid;
    return false;
  }

  return true;
}
