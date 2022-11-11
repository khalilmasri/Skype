#ifndef VIDEO_PLAYBACK_H
#define VIDEO_PLAYBACK_H

#include "playback.hpp"

class  VideoPlayback : public Playback {

  public:

  VideoPlayback();

  void start(P2PPtr &t_p2p_conn, AVStream &t_stream) override;
  void stop() override;
  void load(const Data *t_audio_data) override;
  void show();

  private:
    Webcam             m_webcam;
    Webcam::CVMatQueue m_queue;

  void spawn_video_playback_thread();

};

#endif
