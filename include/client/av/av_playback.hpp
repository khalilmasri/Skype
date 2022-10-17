#ifndef AV_PLAYBACK_H
#define AV_PLAYBACK_H

#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "av_data.hpp"
#include "lock_free_audio_queue.hpp"
#include "peer_to_peer.hpp"
#include "webcam.hpp"
#include <cstddef>
#include <memory>
#include <chrono>

using namespace std::chrono_literals;

class AVPlayback {

  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;
  using P2PPtr = std::unique_ptr<P2P>;

  enum Status { Started, Stopped, Invalid };

public:
  AVPlayback();

  void start(P2PPtr &t_p2p_conn);
  void stop();
  void buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages);

  /* read a single package of av data */
  void read_package_for(P2PPtr &t_p2p_conn, std::size_t nb_frames = 1);
  void read_package(P2PPtr &t_p2p_conn);

private:
  AudioQueuePtr m_audio_queue = std::make_unique<LockFreeAudioQueue>();
  AudioConverter m_audio_converter;
  AudioDevice m_audio_output;
  Status m_status = Stopped;
  Webcam m_webcam;
  Webcam::CVMatQueue m_video_queue;

  void load_audio(const Data *t_audio_data);

/* time counter */

template <
    typename result_t   = std::chrono::milliseconds,
    typename clock_t    = std::chrono::steady_clock,
    typename duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

/* */

  auto valid_data_type(const Data *t_data, Data::Type t_type) -> bool;
  auto valid_read_size(int t_read_size, int t_correct_size) -> bool;

};

#endif
