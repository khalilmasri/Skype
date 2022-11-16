#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include "playback.hpp"
#include <functional>

using namespace std::chrono_literals;

class AudioPlayback : public Playback {

public:
  AudioPlayback();

  void start(P2PPtr &t_p2p_conn, std::function<void()> t_hangup_callback) override;
  void stop() override;
  void load(const Data *t_audio_data) override;

private:
  AudioQueuePtr  m_audio_queue = std::make_unique<LockFreeAudioQueue>();
  AudioConverter m_audio_converter;
  AudioDevice    m_audio_output;
};

#endif
