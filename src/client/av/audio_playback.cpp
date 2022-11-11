#include "audio_playback.hpp"
#include "logger.hpp"

AudioPlayback::AudioPlayback()
    : m_audio_output(m_audio_queue, AudioDevice::Output) {
}

void AudioPlayback::start(P2PPtr &t_p2pconn, AVStream &t_stream) {

  if (m_status == Stopped) {
    m_audio_output.open(); // open audio output queue

    m_done_received = false;
    m_status = Started;
    spawn_network_read_thread(t_p2pconn, t_stream);

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
    LOG_INFO("Closing Audio Playback...");

  } else {
    LOG_ERR("Could not STOP Audio Playback because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

/* */

void AudioPlayback::load(const Data *t_audio_data) {

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


