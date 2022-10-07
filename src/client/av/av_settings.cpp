#include "av_settings.hpp"

/* Video Settings */

AudioSettings *AudioSettings::m_instance = nullptr;
VideoSettings *VideoSettings::m_instance = nullptr;

/* Video Settings */

VideoSettings::VideoSettings(){};

int VideoSettings::bitrate() const { return m_bitrate; }
int VideoSettings::width() const { return m_width; }
int VideoSettings::framerate() const { return m_framerate; }
int VideoSettings::height() const { return m_height; }
int VideoSettings::capture_size_frames() const { return m_capture_size; }

void VideoSettings::delete_instance() { delete m_instance; };

VideoSettings *VideoSettings::get_instance() {

  if (m_instance == nullptr) {
    m_instance = new VideoSettings();
  }

  return m_instance;
};

AudioSettings::AudioSettings() {

  /* we need to grab the fps from video settings here */
  VideoSettings *video_settings = VideoSettings::get_instance();

  /* We must set the converter format according to the device format */
  if (m_device_format == s_Int16Bits) {
    m_bit_multiplier = 2;
    m_converter_format = AV_SAMPLE_FMT_S16;
    m_converter_format_planar = AV_SAMPLE_FMT_S16P;
  } else if (m_device_format == s_Int32Bits) {
    m_bit_multiplier = 4;
    m_converter_format = AV_SAMPLE_FMT_S32;
    m_converter_format_planar = AV_SAMPLE_FMT_S32P;
  } else {
    m_bit_multiplier = 4;
    m_converter_format = AV_SAMPLE_FMT_FLT;
    m_converter_format_planar = AV_SAMPLE_FMT_FLTP;
  }

  /* 44100hz / 25fps * 2 (@ 16bits) = 3528 */
  m_buffer_size = (m_samplerate / video_settings->framerate()) * m_bit_multiplier;
};

int AudioSettings::bitrate() const { return m_bitrate; }
int AudioSettings::samplerate() const { return m_samplerate; }
int AudioSettings::channels() const { return m_channels; }
bool AudioSettings::is_mono() const { return m_channels == 1; }
int AudioSettings::buffer_size() const { return m_buffer_size; }
int AudioSettings::buffer_size_in_samples() const { return m_buffer_size / m_bit_multiplier; }
int AudioSettings::bit_multiplier() const { return m_bit_multiplier; }
int AudioSettings::converter_max_tries() const { return m_converter_max_tries; }
AVCodecID AudioSettings::codec_id() const { return static_cast<AVCodecID>(m_codec_id); }
AVCodecID AudioSettings::codec_id_alt() const { return static_cast<AVCodecID>(m_codec_id_alt); }
AVSampleFormat AudioSettings::converter_format() const { return m_converter_format; }
AVSampleFormat AudioSettings::converter_format_planar() const { return m_converter_format_planar; }


SDL_AudioFormat AudioSettings::device_format() const { return m_device_format; };

void AudioSettings::delete_instance() { delete m_instance; }

AudioSettings *AudioSettings::get_instance() {

  if (m_instance == nullptr) {
    m_instance = new AudioSettings();
  }

  return m_instance;
};
