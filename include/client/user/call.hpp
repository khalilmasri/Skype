#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"
#include "peer_to_peer.hpp"
#include "av_stream.hpp"
#include "audio_playback.hpp"
#include "video_playback.hpp"

#include <cstdint>
#include <memory>
#include <QVector>
#include <functional>

class Call{

public:
   Call();
  ~Call() = default;

  void create(Job &t_job);
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void awaiting(Job &t_job);
  void remove_caller(int t_caller);

  void hangup(Job &t_job);

private:
  using P2PPtr = std::unique_ptr<P2P>;

  QVector<int>   m_callers;
  int            m_current;
  bool           m_hangup  = false;
  bool           m_connected = false;
  std::string    m_token;

  Webcam         m_webcam;
    
  /* Peer to peer connections. One per data stream */
  P2PPtr         m_audio_p2p = nullptr;
  P2PPtr         m_video_p2p = nullptr;

  /* stream objects */
  AVStream       m_audio_stream; 

  AVStream       m_video_stream;
  /* playback objects */
  AudioPlayback  m_audio_playback;
  VideoPlayback  m_video_playback;
  
  /* Establish peer to peer connection */
  auto udp_connect(P2PPtr &t_p2p_conn, Job &t_job, int t_wait_time = 1000) -> bool;
  auto udp_accept(P2PPtr &t_p2p_conn, Job &t_job) -> bool;

  /* Audio */
  void audio_stream();
  void audio_playback();

  /* Video */
  void video_stream();
  void video_playback();

  /* callback */
  auto hangup_callback(AVStream::StreamType t_type) -> std::function<void()>;

  /* Constants */
  inline static const int m_PING_TIMEOUT         = 10;
  inline static const int m_ACCEPT_TIMEOUT       = 40;
  inline static const int m_ACCEPT_THROTTLE_TIME = 60;
  inline static const int m_NB_BUFFER_PACKETS    = 1;

};

#endif // CALL_H
