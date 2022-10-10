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
  using DataCallback =  std::function<void(Data::DataVector&&)>;

  enum Status {Valid, Invalid};

  public:
  AVStream();
  ~AVStream();

  void start();
  void stop();
  void stream(DataCallback &t_callback);

private:
  AudioQueuePtr       m_input_queue;
  AudioQueuePtr       m_output_queue;
  AudioDevice         m_input;
  AudioDevice         m_output;

  AudioConverter     m_converter;
  Webcam             m_webcam;
};

#endif
