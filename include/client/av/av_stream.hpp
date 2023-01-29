#ifndef AV_STREAM_H
#define AV_STREAM_H

#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "audio_device_config.hpp"
#include "av_settings.hpp"
#include "lock_free_audio_queue.hpp"
#include "webcam.hpp"
#include "av_data.hpp"
#include <thread>
#include "peer_to_peer.hpp"

#include <functional>

class AVStream {
  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;
  enum Status {Started, Stopped, Invalid};
  using P2PPtr = std::unique_ptr<P2P>;

public:
  enum StreamType {Audio, Video};

  AVStream(Webcam &t_webcam, StreamType t_type = Audio);
  using StreamCallback =  std::function<void(Data::DataVector&&)>;
 // ~AVStream();

  void start();
  void stop();
  void stream(P2PPtr &p2p_conn);
  void set_stream_type(StreamType t_type);
  StreamType stream_type() const;

private:
  AudioQueuePtr      m_input_queue = std::make_unique<LockFreeAudioQueue>(); 
  AudioDevice        m_input;
  Status             m_status = Stopped;

  AudioConverter     m_converter;
 Webcam              &m_webcam; // webcam lives in the parent call class;
  StreamType         m_stream_type;

  void validate();
  void stream_audio(P2PPtr &t_p2p_conn); /* spawns a thread streaming audio */
  void stream_video(P2PPtr &t_p2p_conn); /* spawns a thread streaming video */
  void send_audio(P2PPtr &t_p2p_conn); 
  void send_done(P2PPtr &t_p2p_conn);
};

#endif
