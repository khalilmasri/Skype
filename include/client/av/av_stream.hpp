#ifndef AV_STREAM_H
#define AV_STREAM_H

#include "audio_converter.hpp"
#include "audio_device.hpp"
#include "audio_device_config.hpp"
#include "av_settings.hpp"
#include "lock_free_audio_queue.hpp"
#include "webcam.hpp"
#include "av_data.hpp"

#include <functional>

class AVStream {
  using AudioQueuePtr = std::unique_ptr<LockFreeAudioQueue>;

  enum Status {Started, Stopped, Invalid};

  public:


  AVStream();
  using DataCallback =  std::function<void(Webcam::WebcamFrames&&, Data::DataVector&&)>;
  ~AVStream()=default;

  void start();
  void stop();
  void stream(DataCallback &t_callback);

private:
  AudioQueuePtr       m_input_queue;
  AudioDevice         m_input;
  Status              m_status = Stopped;

  AudioConverter     m_converter;
  Webcam             m_webcam;

  void validate();
};

#endif
