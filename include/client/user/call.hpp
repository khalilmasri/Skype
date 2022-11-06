#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"
#include "peer_to_peer.hpp"
#include "av_stream.hpp"
#include "audio_playback.hpp"

#include <cstdint>
#include <memory>
#include <QVector>

class Call{

public:
  ~Call()= default;
  
  void create(Job &t_job);
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void awaiting(Job &t_job);
  void remove_caller(int t_caller);

  /* Audio video IO */
  void av_stream();
  void av_playback();
  /* */

  void webcam();
  void mute();
  void hangup();

private:
  QVector<int> m_callers;
  int          m_current;

  using P2PPtr = std::unique_ptr<P2P>;

  bool           m_hangup   = false;
  bool           m_webcam   = false;
  bool           m_mute     = false;
  P2PPtr         m_audio_p2p = nullptr;
  AVStream       m_stream; // stream will initialize as Audio but can be initialized as Video.
  AudioPlayback  m_playback;

 auto stream_callback() -> AVStream::StreamCallback;

  inline static const int m_TIMEOUT = 10;
};

#endif // CALL_H
