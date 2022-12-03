#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "av_data.hpp"
#include "av_stream.hpp"
#include "lock_free_audio_queue.hpp"
#include "peer_to_peer.hpp"
#include "webcam.hpp"
#include <memory>
#include <thread>

class Playback {

public:
  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;
  using P2PPtr = std::unique_ptr<P2P>;

  enum Status { Started, Stopped, Invalid };

  /* common members */

  bool m_done_received = false;
  Status m_status = Stopped;

  /* virtual implementation */

  virtual ~Playback() = default;
  virtual void start(P2PPtr &t_p2p_conn, std::function<void()> t_hangup_callback) = 0;
  virtual void stop() = 0;
  virtual void load(const Data *data) = 0;

  /* common implementation (see playback.cpp) */

  void buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages);
  void read_package(P2PPtr &t_p2pconn);
  void spawn_network_read_thread(P2PPtr &t_p2p_conn, std::function<void()> t_hangup_callback);
  auto valid_data_type(const Data *t_data, Data::Type t_type) -> bool;

  inline static const std::size_t m_MAX_TRIES = 100;
  inline static const std::size_t m_WARNING_TRIES = 300;
};

#endif
