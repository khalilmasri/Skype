#include "audio_converter.hpp"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <libavcodec/avcodec.h>
#include <libavcodec/defs.h>
#include <libavutil/samplefmt.h>
#include <thread>

AudioSettings *AudioConverter::m_AUDIO_SETTINGS = AudioSettings::get_instance();
VideoSettings *AudioConverter::m_VIDEO_SETTINGS = VideoSettings::get_instance();

/* Constructor */

AudioConverter::AudioConverter()
    : m_decoded_data(m_AUDIO_SETTINGS->buffer_size()) {

  m_encoder_codec = avcodec_find_encoder(m_AUDIO_SETTINGS->codec_id());
  m_decoder_codec = avcodec_find_decoder(m_AUDIO_SETTINGS->codec_id());

  is_valid_pointer(m_encoder_codec,
                   "Could not find encoder codec. Selecting alternative...");
  is_valid_pointer(m_decoder_codec,
                   "Could not find decoder codec. Selecting alternative...");

  m_AUDIO_SETTINGS->codec_id_alt();

  if (!m_valid) {
    m_valid = true;
    m_encoder_codec = avcodec_find_encoder(m_AUDIO_SETTINGS->codec_id_alt());
    m_decoder_codec = avcodec_find_decoder(m_AUDIO_SETTINGS->codec_id_alt());
    is_valid_pointer(m_encoder_codec, "Could find encoder codec.");
    is_valid_pointer(m_decoder_codec, "Could find decoder codec");
  }

  create_encoder_context();
  create_decoder_context();

  if (m_valid) {
    m_parser = av_parser_init(m_decoder_codec->id);
    is_valid_pointer(m_parser, "Failed to initialize decoder parser.");
  }

  if (m_valid) {
    m_encode_packet = av_packet_alloc();
    m_decode_packet = av_packet_alloc();
    is_valid_pointer(m_encode_packet, "Could not allocate encode packet.");
    is_valid_pointer(m_decode_packet, "Could not allocate decoder packet.");
  }

  if (m_valid) {
    setup_frame();
  }
}

/* Destructor */

AudioConverter::~AudioConverter() { // free resources

  if (m_encoder_context) {
    avcodec_free_context(&m_encoder_context);
  }

  if (m_decoder_context) {
    avcodec_free_context(&m_decoder_context);
  }

  if (m_parser) {
    av_parser_close(m_parser);
  }

  if (m_frame) {
    av_frame_free(&m_frame);
  }

  if (m_decode_frame) {
    av_frame_free(&m_decode_frame);
  }

  if (m_flush_frame) {
    av_frame_free(&m_flush_frame);
  }

  if (m_encode_packet) {
    av_packet_free(&m_encode_packet);
  }

  if (m_decode_packet) {
    av_packet_free(&m_decode_packet);
  }
}

/* Public */

std::size_t AudioConverter::frame_size_bytes() { // in bytes

  int result = av_samples_get_buffer_size(
      NULL, m_encoder_context->ch_layout.nb_channels,
      m_encoder_context->frame_size, m_encoder_context->sample_fmt, 0);

  if (is_valid(result, "Could not get the converter frame size.")) {
    return result;
  } else {
    return 0;
  }
}

/* */

std::size_t AudioConverter::frame_size_samples() const {
  return m_encoder_context->frame_size;
}

/* */

bool AudioConverter::valid() const { return m_valid; }

/* */

std::vector<uint8_t> AudioConverter::encode(AudioQueue &t_queue) {

  std::size_t capture_size = m_VIDEO_SETTINGS->capture_size_frames();
  std::vector<uint8_t> data;
  int tries = m_AUDIO_SETTINGS->converter_max_tries();

  data.reserve(
      capture_size *
      m_AUDIO_SETTINGS->buffer_size()); // allocating more we need so vector
                                        // don't allocate at every push_back.

  for (std::size_t i = 0; i < capture_size; i++) {
    if (!t_queue->empty()) {

      auto package = t_queue->pop();
      encode_package(package, data);

    } else { // try again
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      i--;
      tries--;
    }

    // will try to pull from queue a couple times and give up.
    if (tries <= 0) {
      // TODO Log this error
      std::cout << "Conversion did not complete. Queue is empty.\n";
      break;
    }
  }

  encode_frames(data, true); // flush encoder.
  create_encoder_context();  // create next context for next conversion //
  m_awaiting = false;
  return data;
}

/* */

bool AudioConverter::decode(AudioQueue &t_queue,
                            std::vector<uint8_t> t_encoded_data) {

  uint8_t *data = t_encoded_data.data();
  std::size_t data_size = t_encoded_data.size();

  while (data_size > 0) {

    if (!m_valid) {
      return m_valid;
    }

    if (!m_decode_frame) {
      m_decode_frame = av_frame_alloc();
      is_valid_pointer(m_decode_frame, "Could not allocate decode frame.");
    }

    int result =
        av_parser_parse2(m_parser, m_decoder_context, &m_decode_packet->data,
                         &m_decode_packet->size, data, data_size,
                         AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

    if (!is_valid(result, "Decoder could not parse input.")) {
      return m_valid;
    }

    data += result; // manipulate pointer to new position
    data_size -= result;

    if (m_decode_packet->size) {
      decode_frames(
          t_queue); // will push to the queue when m_decoded_data is full
    }
  }

  if (m_awaiting) { // flush decoder
    m_decode_packet->data = nullptr;
    m_decode_packet->size = 0;
    decode_frames(t_queue);
    m_awaiting = false;
    create_decoder_context(); // create a new context for the next conversion
  }

  return m_valid;
}

/* Private */

void AudioConverter::encode_package(std::shared_ptr<AudioPackage> &t_package,
                                    std::vector<uint8_t> &t_data) {

  // here we get how many ffmpeg frames we need to convert a given SDL buffer
  // size.
  std::size_t frame_size_in_bytes = frame_size_bytes();

  // 3528 / 2304 = 2

  if (m_valid) {
    std::size_t nb_frames = std::ceil(t_package->m_len / frame_size_in_bytes);

    // offsets by the frame size at iteration.
    for (std::size_t i = 0; i < nb_frames; i++) {
      std::size_t package_offset = frame_size_in_bytes * i;

      copy_to_frame(t_package->m_data.data(), frame_size_in_bytes,
                    package_offset);

      if (m_valid) {
        encode_frames(t_data);
      }
    }
  }
}

/* */

void AudioConverter::encode_frames(std::vector<uint8_t> &t_data, bool t_flush) {

  if (!m_valid) {
    return;
  }

  int result;

  if (t_flush) { // pass in a null AVFrame pointer to flush encoder.
    result = avcodec_send_frame(m_encoder_context, m_flush_frame);
  } else {
    result = avcodec_send_frame(m_encoder_context,
                                m_frame); // send frame for encoding
  };

  is_valid(result, "Sending frame for encoding failed.");

  if (m_valid) {

    while (result >= 0) { // there may be many number of packets to read

      if (!m_valid) {
        break;
      }

      result = avcodec_receive_packet(m_encoder_context, m_encode_packet);

      if (result == AVERROR(EAGAIN)) {
        //  Not enough data to conver. waiting on the next frame
        m_awaiting = true;
        break;
      }

      else if (result == AVERROR_EOF) {
        // converter is done
        m_awaiting = false;
        break;
      }

      else if (!is_valid(result, "Error encoding frame.")) {
        break;
      }

      for (std::size_t i = 0; i < m_encode_packet->size;
           i++) { // push to output
        t_data.push_back(m_encode_packet->data[i]);
      }

      m_awaiting = false;
      av_packet_unref(m_encode_packet); // clean up packet
    }
  }
}

void AudioConverter::decode_frames(AudioQueue &m_queue) {

  std::size_t data_size = 0;
  int result = avcodec_send_packet(m_decoder_context, m_decode_packet);

  is_valid(result, "Could not send packet to decode.");

  while (result >= 0) {

    result = avcodec_receive_frame(m_decoder_context, m_decode_frame);

    if (result == AVERROR(EAGAIN)) {
      // needs more data to convert.
      m_awaiting = true;
      break;
    }

    else if (result == AVERROR_EOF) {
      // converted is done!
      m_awaiting = false;
      break;
    }

    else if (!is_valid(result, "Error encoding frame.")) {
      break;
    }

    data_size = av_get_bytes_per_sample(m_decoder_context->sample_fmt);

    if (!is_valid(data_size, "Failed to calculate sample size in decoder.")) {
      break;
    }

    for (int i = 0; i < m_decode_frame->nb_samples * data_size; i++) {

      //  It will always be mono for skype
      for (int ch = 0; ch < m_decoder_context->ch_layout.nb_channels; ch++) {
          uint8_t *data = (uint8_t*)m_decode_frame->data[ch];
          uint8_t byte = data[i];
        if (m_decoded_data.m_index >= m_decoded_data.m_len) {
          m_queue->push(std::move(m_decoded_data));
          m_decoded_data = AudioPackage(m_AUDIO_SETTINGS->buffer_size());
        }

        m_decoded_data.push_back(byte);
      }
    }
  }
}

/* */

void AudioConverter::create_encoder_context() {

  if (m_encoder_context) { // free previous contexts.
    avcodec_free_context(&m_encoder_context);
  }

  if (m_valid) {
    m_encoder_context = avcodec_alloc_context3(m_encoder_codec);
    is_valid_pointer(m_encoder_context, "Could not allocate encoder context.");
  }

  if (m_valid) {
    m_encoder_context->bit_rate = m_AUDIO_SETTINGS->bitrate();
    m_encoder_context->sample_fmt = m_AUDIO_SETTINGS->converter_format();
    validate_sample_format();
  }

  if (m_valid) {
    m_encoder_context->sample_rate = m_AUDIO_SETTINGS->samplerate();
    validate_sample_rate();
  }

  if (m_valid) {
    set_channel_layout();
    int result = avcodec_open2(m_encoder_context, m_encoder_codec, NULL);
    is_valid(result, "Could not open encoder codec.");
  }
}

void AudioConverter::create_decoder_context() {

  if (m_decoder_context) { // free previous contexts.
    avcodec_free_context(&m_decoder_context);
  }

  if (m_valid) {
    m_decoder_context = avcodec_alloc_context3(m_decoder_codec);
    is_valid_pointer(m_decoder_context, "Could not allocate decoder context.");
  }

  if (m_valid) {
    int result = avcodec_open2(m_decoder_context, m_decoder_codec, NULL);
    is_valid(result, "Could not open decoder codec.");
  }
}

void AudioConverter::set_channel_layout() {

  AVChannelLayout layout;

  if (m_AUDIO_SETTINGS->is_mono()) {
    layout = AV_CHANNEL_LAYOUT_MONO;
  } else {

    layout = AV_CHANNEL_LAYOUT_STEREO;
  }

  av_channel_layout_copy(&m_encoder_context->ch_layout, &layout);
}

void AudioConverter::setup_frame() {

  m_frame = av_frame_alloc();
  is_valid_pointer(m_frame, "Could not allocate frame.");

  if (m_valid) {

    m_frame->nb_samples = m_encoder_context->frame_size;
    m_frame->format = m_encoder_context->sample_fmt;
    int result = av_channel_layout_copy(&m_frame->ch_layout,
                                        &m_encoder_context->ch_layout);

    is_valid(result, "Could not copy channel layout to frame.");
  }

  if (m_valid) {
    int result = av_frame_get_buffer(m_frame, 0);
    is_valid(result, "Could not allocate audio data buffer");
  }
}

/* */

void AudioConverter::copy_to_frame(uint8_t *t_audio_buffer,
                                   std::size_t t_frame_size_bytes,
                                   std::size_t t_offset) {

  int result = av_frame_make_writable(m_frame);

  is_valid(result, "Could not make frame writable.");
  uint8_t *buffer = m_frame->data[0];

  if (m_valid) {
    for (std::size_t i = 0; i < t_frame_size_bytes; i++) {
      buffer[i] = t_audio_buffer[t_offset + i]; // mono channel only.
    }
  }
}

void AudioConverter::write_zeros_to_frame(std::size_t t_frame_size_bytes,
                                          std::size_t t_offset) {

  int result = av_frame_make_writable(m_frame);

  is_valid(result, "Could not make frame writable.");
  uint8_t *buffer = m_frame->data[0];

  if (m_valid) {
    // write 0s from offset to end of the buffer
    for (std::size_t i = t_offset; i < t_frame_size_bytes; i++) {
      buffer[i] = 0;
    }
  }
}

/* */

bool AudioConverter::validate_sample_rate() {

  if (m_encoder_codec->supported_samplerates) {
    for (int i = 0; m_encoder_codec->supported_samplerates[i]; i++) {
      if (m_encoder_context->sample_rate ==
          m_encoder_codec->supported_samplerates[i]) {
        return m_valid;
      }
    }

    int next_supported = m_encoder_codec->supported_samplerates[0];

    std::cout << "Could not validate sample rate. Selecting next available:  "
              << next_supported << "\n";

    m_encoder_context->sample_rate = next_supported;
  }


  return m_valid;
}

/* */

bool AudioConverter::validate_sample_format() {

  const enum AVSampleFormat *sample_formats = m_encoder_codec->sample_fmts;
  const enum AVSampleFormat *first_format = sample_formats;

  while (*sample_formats != AV_SAMPLE_FMT_NONE) {
    if (*sample_formats == m_encoder_context->sample_fmt) {
      return m_valid;
    }

    sample_formats++;
  }

  // if sample format is not available for the codec try the planar alternative.
  sample_formats = first_format;
  AVSampleFormat planar_alternative =
      m_AUDIO_SETTINGS->converter_format_planar();

  while (*sample_formats != AV_SAMPLE_FMT_NONE) {
    if (*sample_formats == planar_alternative) {
      m_encoder_context->sample_fmt = *sample_formats;
      return m_valid;
    }

    sample_formats++;
  }

  // TODO: LOG
  std::cout << "Error: Could not set a valid sample format for this codec.\n";
  m_valid = false;

  return m_valid;
}

/* */

template <typename T>
bool AudioConverter::is_valid_pointer(T t_pointer, const char *t_msg) {

  if (t_pointer == nullptr) {
    std::cout << t_msg << "\n";
    m_valid = false;
  }

  return m_valid;
}

/* */

bool AudioConverter::is_valid(int t_result, const char *t_msg) {
  if (t_result < 0) {

    // TODO: LOG Error
    m_valid = false;
    std::cout << t_msg << "\n";
  }

  return m_valid;
}
