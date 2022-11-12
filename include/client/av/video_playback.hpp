#ifndef VIDEO_PLAYBACK_H
#define VIDEO_PLAYBACK_H

#include "playback.hpp"

class  VideoPlayback : public Playback {

  public:

  VideoPlayback(Webcam &t_webcam);

  void start(P2PPtr &t_p2p_conn, AVStream &t_stream) override;
  void stop() override;
  void load(const Data *t_audio_data) override;
  void show();

  private:
    Webcam             &m_webcam; // webcam will live in the parent call class.
    Webcam::CVMatQueue m_queue;

  void spawn_video_playback_thread();

  inline static const std::size_t m_MID_PLAYBACK_THROTTLE = 30;
  inline static const std::size_t m_MAX_PLAYBACK_THROTTLE = 100;

};

#endif
