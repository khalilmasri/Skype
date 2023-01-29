#include "audio_device.hpp"
#include "audio_device_config.hpp"
#include "logger.hpp"

#include <iostream>

AudioDevice::AudioDevice(std::unique_ptr<LockFreeAudioQueue> &t_queue, Type t_type) {

  SDL_AudioSpec want;
  SDL_AudioSpec have;

  auto *config_audio = AudioSettings::get_instance();

  AudioDevConfig *config = AudioDevConfig::get_instance();

  SDL_zero(want);

  want.freq = config_audio->samplerate();
  want.format = config_audio->device_format();
  want.channels = config_audio->channels();
  want.callback = t_type == Input ? audio_input_callback : audio_output_callback;
  want.samples = config_audio->buffer_size_in_samples(); // audio buffer is = 1 frame of video.
  want.userdata = t_queue.get();

  // HERE: The program will print the input and output being used. 
  // if not the correct one please select with the following functions
  // config->select_output(1);
  //config->select_input(0);

  // config->select_output(1); // this is to work on pedros computer! check yours.

  // HERE: You can also print your list of ins and outs like so:
//  auto outputs = config->list_output_name();
//  for(auto &output: outputs) { std::cout << output << "\n"; }

 // config->select_input(0); // <----- THIS !
 // config->select_output(0); // <----- THIS !
  
  std::string interface_name = t_type == Input ? config->get_input().c_str()
                                               : config->get_output().c_str();

  const char *in_or_out = t_type == Input ? "Input" : "Output";


  LOG_INFO("Selecting Audio Device %s : %s\n", in_or_out,
          interface_name.c_str());

  m_dev = SDL_OpenAudioDevice(interface_name.c_str(), static_cast<int>(t_type), &want, &have,
      0);

  if (m_dev == 0) {
    m_status = Invalid;
    SDL_Log("Failed to open audio: %s", SDL_GetError());
  };

  log_on_mismatch_audiospec(want, have);
}

AudioDevice::~AudioDevice() { SDL_CloseAudioDevice(m_dev); }

void AudioDevice::open() {
  if (m_status == Closed) {
    SDL_PauseAudioDevice(m_dev, 0);
    m_status = Opened;
  } else {
     LOG_DEBUG("Audio device is already opened.");
  }
};

void AudioDevice::close() {
  if (m_status == Opened) {
    SDL_PauseAudioDevice(m_dev, 1);
    m_status = Closed;
  } else {
     LOG_DEBUG("Audio device is already closed.");
  } 
}

void AudioDevice::wait(int t_frames) {
  auto *config_video = VideoSettings::get_instance();
  SDL_Delay(1000 / config_video->framerate() * t_frames);
};

void AudioDevice::start_sdl(){
  Uint32 subsystem_init = 0;
  if (SDL_WasInit(subsystem_init & SDL_INIT_AUDIO) <= 0) {
    SDL_Init(SDL_INIT_AUDIO);
  }
}

void AudioDevice::log_on_mismatch_audiospec(SDL_AudioSpec t_want, SDL_AudioSpec t_have) {

  if (t_have.channels != t_want.channels) {
    SDL_Log("Audio spec didn't match. Want: %i have: %i\n", t_want.channels,
            t_have.channels);
  }

  if (t_have.format != t_want.format) {
    SDL_Log("Format didn't match. Want: %i have: %i\n", t_want.format,
            t_have.format);
  }

  if (t_have.samples != t_want.samples) {
    SDL_Log("Buffer size didn't match. Want: %i have: %i\n", t_want.samples,
            t_have.samples);
  }
}

void AudioDevice::audio_input_callback(void *user_data, Uint8 *stream, int len) {

  auto *queue = static_cast<LockFreeAudioQueue *>(user_data);
  queue->push(AudioPackage(stream, len));
};

void AudioDevice::audio_output_callback(void *user_data, Uint8 *stream, int len) {

  auto *queue = static_cast<LockFreeAudioQueue *>(user_data);

  auto audio = queue->pop();

  if (audio && audio->m_len <= len) {
    for (int i = 0; i < audio->m_len; i++) {
      stream[i] = audio->m_data[i];
    }
  };
};
