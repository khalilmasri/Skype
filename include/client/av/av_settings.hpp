#ifndef AV_SETTINGS_H
#define AV_SETTINGS_H

extern "C" { // required by linker
#include <libavcodec/avcodec.h>
}

#include <SDL2/SDL_audio.h>
#include <string>

/* audio */

class AudioSettings {

  enum CodecID {
    mp3 = AV_CODEC_ID_MP3,
    mp3_alt = AV_CODEC_ID_MP3ADU,
    mp2 = AV_CODEC_ID_MP2,
    vorbis = AV_CODEC_ID_VORBIS,
    opus = AV_CODEC_ID_OPUS,
    flac = AV_CODEC_ID_FLAC,
    aac = AV_CODEC_ID_AAC,
    ac3 = AV_CODEC_ID_AC3
  };

  enum Format {
    s_Int16Bits = AUDIO_S16,
    s_Int32Bits = AUDIO_S32,
    s_float32Bits = AUDIO_F32,
  };

public:
  static auto get_instance() -> AudioSettings*;

  static void delete_instance();

  [[nodiscard]] auto samplerate() const -> int;
  [[nodiscard]] auto bitrate() const -> int;
  [[nodiscard]] auto device_bitrate() const -> int;
  [[nodiscard]] auto channels() const -> int;
  [[nodiscard]] auto is_mono() const -> bool;
  [[nodiscard]] auto buffer_size() const -> int;
  [[nodiscard]] auto buffer_size_in_samples() const -> int;
  [[nodiscard]] auto bit_multiplier() const -> int;
  [[nodiscard]] auto converter_max_tries() const -> int;
  [[nodiscard]] auto converter_format() const -> AVSampleFormat;
  // the planar format just means that stereo channels are interleaved.
  [[nodiscard]] auto converter_format_planar() const -> AVSampleFormat;
  [[nodiscard]] auto device_format() const -> SDL_AudioFormat;
  [[nodiscard]] auto codec_id() const -> AVCodecID;
  [[nodiscard]] auto codec_id_alt() const -> AVCodecID;

  AudioSettings(AudioSettings &lhs) = delete;
  void operator=(AudioSettings &lhs) = delete;

private:
  static AudioSettings *m_instance;

  CodecID m_codec_id = mp3;
  CodecID m_codec_id_alt = mp3_alt;                  // alternative in case first option is not avail.
  int m_channels = 1;
  int m_bitrate = 64000;
  int m_samplerate = 44100;
  SDL_AudioFormat m_device_format = s_float32Bits;
  AVSampleFormat m_converter_format;            // will be set according to m_device_format
  AVSampleFormat m_converter_format_planar;     // will be set according to m_device_format
  int m_bit_multiplier;                         //  Will be set according to m_device_format
  int m_buffer_size;                            //  will be set in the constructor
  int m_converter_max_tries = 40;

  AudioSettings();
};

/* Video */

class VideoSettings {

public:
  static auto get_instance() -> VideoSettings *;

  static void delete_instance();

  VideoSettings(VideoSettings &lhs) = delete;
  void operator=(VideoSettings &lhs) = delete;

  [[nodiscard]] auto bitrate() const -> int;
  [[nodiscard]] auto width() const -> int;
  [[nodiscard]] auto height() const -> int;
  [[nodiscard]] auto framerate() const -> int;
  [[nodiscard]] auto capture_size_frames() const -> int;
  [[nodiscard]] auto converter_type() const -> std::string;
  [[nodiscard]] auto camera() -> int;
  void set_camera(int t_camera);

private:
  static VideoSettings *m_instance;

  std::string m_converter_type = ".jpeg";
  int m_bitrate = 4000;
  int m_width = 320;
  int m_height = 180;
  int m_framerate = 25;
  int m_capture_size = 1; // video frames @ 25 fps
  int m_camera = 0;

  VideoSettings();
};

#endif
