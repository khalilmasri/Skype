#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "av_data.hpp"
#include "lock_free_audio_queue.hpp"
#include "peer_to_peer.hpp"
#include "webcam.hpp"
#include <memory>
#include <thread>
#include "av_stream.hpp"


using namespace std::chrono_literals;

#define MAX_EMPTY_PACKAGES 10

class AudioPlayback {

  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;
  using P2PPtr = std::unique_ptr<P2P>;

  enum Status { Started, Stopped, Invalid };

public:
  AudioPlayback();

  void start(P2PPtr &t_p2p_conn, AVStream &t_stream);
  void stop();
  void buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages);

  /* read a single package of av data */
  void read_package(P2PPtr &t_p2p_conn);

private:
  AudioQueuePtr  m_audio_queue = std::make_unique<LockFreeAudioQueue>();
  AudioConverter m_audio_converter;
  AudioDevice    m_audio_output;
  Status         m_status = Stopped;
  bool           m_done_received = false;

  void load_audio(const Data *t_audio_data);
  auto valid_data_type(const Data *t_data, Data::Type t_type) -> bool;

};

#endif
