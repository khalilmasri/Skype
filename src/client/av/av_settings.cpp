#include "av_settings.hpp"

/* Video Settings */

AudioSettings *AudioSettings::m_instance = nullptr;
VideoSettings *VideoSettings::m_instance = nullptr;

/* Video Settings */

VideoSettings::VideoSettings() = default;

auto VideoSettings::bitrate() const -> int { return m_bitrate; }
auto VideoSettings::width() const -> int { return m_width; }
auto VideoSettings::framerate() const -> int { return m_framerate; }
auto VideoSettings::height() const -> int { return m_height; }
auto VideoSettings::capture_size_frames() const -> int { return m_capture_size; }
auto VideoSettings::converter_type() const -> std::string { return m_converter_type; }
void VideoSettings::delete_instance() { delete m_instance; };
auto VideoSettings::camera() -> int { return m_camera; };

auto VideoSettings::get_instance() -> VideoSettings * {

  if (m_instance == nullptr) {
    m_instance = new VideoSettings();
  }

  return m_instance;
};

void VideoSettings::set_camera(int t_camera)
{
  m_camera = t_camera;
}

AudioSettings::AudioSettings() {

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

   /* 44100hz / 25fps * 4 (@ 32bits) = 7056 */
  //  m_buffer_size = (m_samplerate / video_settings->framerate()) * m_bit_multiplier; // 7056
  
   /* based on ffmpeg frame size */
    m_buffer_size = 1152 * m_bit_multiplier; // 4608
    // 
};

auto AudioSettings::bitrate() const -> int { return m_bitrate; }
auto AudioSettings::samplerate() const -> int { return m_samplerate; }
auto AudioSettings::channels() const -> int { return m_channels; }
auto AudioSettings::is_mono() const -> bool { return m_channels == 1; }
auto AudioSettings::buffer_size() const -> int { return m_buffer_size; }
auto AudioSettings::buffer_size_in_samples() const -> int { return m_buffer_size / m_bit_multiplier; }
auto AudioSettings::bit_multiplier() const -> int { return m_bit_multiplier; }
auto AudioSettings::converter_max_tries() const -> int { return m_converter_max_tries; }
auto AudioSettings::codec_id() const -> AVCodecID { return static_cast<AVCodecID>(m_codec_id); }
auto AudioSettings::codec_id_alt() const -> AVCodecID { return static_cast<AVCodecID>(m_codec_id_alt); }
auto AudioSettings::converter_format() const -> AVSampleFormat { return m_converter_format; }
auto AudioSettings::converter_format_planar() const -> AVSampleFormat { return m_converter_format_planar; }
auto AudioSettings::device_format() const -> SDL_AudioFormat { return m_device_format; };
void AudioSettings::delete_instance() { delete m_instance; }

auto AudioSettings::get_instance() -> AudioSettings * {
  if (m_instance == nullptr) {
    m_instance = new AudioSettings();
  }

  return m_instance;
};
