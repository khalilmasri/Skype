#ifndef AV_SETTINGS_H
#define AV_SETTINGS_H

extern "C" { // required by linker
#include <libavcodec/avcodec.h>
}

#include <SDL2/SDL_audio.h>

/* audio */

class AudioSettings {

  enum CodecID {
    mp3 = AV_CODEC_ID_MP3,
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
  static AudioSettings *get_instance();

  static void delete_instance();

  int samplerate() const;
  int bitrate() const;
  int device_bitrate() const;
  int channels() const;
  bool is_mono() const;
  int buffer_size() const;
  int buffer_size_in_samples() const;
  int bit_multiplier() const;
  int converter_max_tries() const;
  AVSampleFormat converter_format() const;

  // the planar format just means that stereo channels are interleaved.
  AVSampleFormat converter_format_planar() const;
  SDL_AudioFormat device_format() const;
  AVCodecID codec_id() const;
  AVCodecID codec_id_alt() const;

  AudioSettings(AudioSettings &lhs) = delete;
  void operator=(AudioSettings &lhs) = delete;

private:
  static AudioSettings *m_instance;

  CodecID m_codec_id = mp3;
  CodecID m_codec_id_alt = mp2; // alternative in case first option is not avail.
  int m_channels = 1;
  int m_bitrate = 64000;
  int m_samplerate = 44100;
  SDL_AudioFormat m_device_format = s_float32Bits;
  AVSampleFormat m_converter_format;         // will be set according to m_device_format
  AVSampleFormat m_converter_format_planar;  // will be set according to m_device_format
  int m_bit_multiplier;                      //  Will be set according to m_device_format
  int m_buffer_size;                         //  will be set in the constructor
  int m_converter_max_tries = 10;

  AudioSettings();
};

/* Video */

class VideoSettings {

public:
  static VideoSettings *get_instance();

  static void delete_instance();

  VideoSettings(VideoSettings &lhs) = delete;
  void operator=(VideoSettings &lhs) = delete;

  int bitrate() const;
  int width() const;
  int height() const;
  int framerate() const;
  int capture_size_frames() const;

private:
  static VideoSettings *m_instance;

  int m_bitrate = 40000;
  int m_height = 480;
  int m_width = 640;
  int m_framerate = 25;
  int m_capture_size = 5; // video frames @ 25 fps

  VideoSettings();
};

#endif
