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

#include <functional>

class AVStream {
  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;

  enum Status {Started, Stopped, Invalid};

public:
  enum StreamType {Audio, Video};

  AVStream(StreamType t_type = Audio);
  using StreamCallback =  std::function<void(Data::DataVector&&)>;
 // ~AVStream();

  void start();
  void stop();
  void stream(StreamCallback &&t_callback);
  void set_stream_type(StreamType t_type);

private:
  AudioQueuePtr       m_input_queue = std::make_unique<LockFreeAudioQueue>(); 
  AudioDevice         m_input;
  Status              m_status = Stopped;

  AudioConverter     m_converter;
  Webcam             m_webcam;
  StreamType         m_stream_type;
  StreamCallback     m_callback;

  void validate();
};

#endif
