#ifndef AUDIO_CONVERTER_H
#define AUDIO_CONVERTER_H

extern "C" { // must use to link
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

#include <vector>
#include "av_settings.hpp"
#include "av_data.hpp"
#include "lock_free_audio_queue.hpp"

class AudioConverter {

  using AudioQueue = std::unique_ptr<LockFreeAudioQueue>;

public:
  AudioConverter();

  auto frame_size_bytes() -> std::size_t;
  [[nodiscard]] auto frame_size_samples() const -> std::size_t ;
  [[nodiscard]] auto valid() const -> bool;

  void make_valid(const char *t_from);

  auto encode(AudioQueue &t_queue) -> Data::DataVector;
  auto decode(Data::DataVector &t_encoded_data) -> Data::DataVector;

  ~AudioConverter();

  const AVCodec*          m_encoder_codec    = nullptr;
  const AVCodec*          m_decoder_codec    = nullptr;

private:
  AVCodecContext*         m_encoder_context  = nullptr;
  AVCodecContext*         m_decoder_context  = nullptr;
  AVCodecParserContext*   m_parser           = nullptr;
  AVFrame*                m_frame            = nullptr;
  AVFrame*                m_decode_frame     = nullptr;
  AVFrame*                m_flush_frame      = nullptr;
  AVPacket*               m_encode_packet    = nullptr;
  AVPacket*               m_decode_packet    = nullptr;
  AudioPackage            m_decoded_data;
  bool                    m_valid            = true;
  bool                    m_awaiting         = false;
  int                     m_decoder_byte_count = 0;

  // to track if we are decoding the first frame of a AudioConverter::encode call
  bool                    m_first_frame      = true;


  void encode_package(std::shared_ptr<AudioPackage> &t_package, std::vector<uint8_t> &t_data);
  void encode_frames(std::vector<uint8_t> &t_data, bool t_flush = false);

  void decode_frames(Data::DataVector &decoded_data);

  void create_encoder_context();
  void create_decoder_context();

  void print_sample_values(uint8_t *t_audio_buffer, int beg, int end);
  void print_sample_values_in_bytes(uint8_t *t_audio_buffer, int beg, int end);

  void set_channel_layout();
  void setup_frame();
  void copy_to_frame(const std::shared_ptr<AudioPackage> &t_audio_buffer, std::size_t t_frame_size_bytes, std::size_t t_offset);
  void write_zeros_to_frame(std::size_t t_frame_size_bytes, std::size_t t_offset);

  void clean_buffer_tail(Data::DataVector &t_data);

  auto validate_sample_format() -> bool;
  auto validate_sample_rate() -> bool;
  
  template<typename T>
  auto is_valid_pointer(T t_pointer, const char *t_msg) -> bool;
  auto is_valid(int t_result, const char *t_msg) -> bool;


 // static AudioSettings *m_AUDIO_SETTINGS;
 // static VideoSettings *m_VIDEO_SETTINGS;
};

#endif
