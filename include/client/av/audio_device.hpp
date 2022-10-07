#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#define INPUT_AUDIO_DEVICES 1
#define OUTPUT_AUDIO_DEVICES 0

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_types.h>
#include <functional>
#include <memory>

#include "lock_free_audio_queue.hpp"
#include "av_settings.hpp"


class AudioDevice {

  public:
  enum Status { Invalid, Closed, Opened };
  enum Type { Output = 0, Input = 1};

  AudioDevice(std::unique_ptr<LockFreeAudioQueue> &t_queue, Type t_type);
  ~AudioDevice();
  void open();
  void close();
  void wait(int t_frames);

  private:
    SDL_AudioDeviceID m_dev        = 0;
    Status            m_status     = Closed;
    

    void log_on_mismatch_audiospec(SDL_AudioSpec t_want, SDL_AudioSpec t_have);

    static void audio_input_callback(void *user_data, Uint8 *stream, int len);
    static void audio_output_callback(void *user_data, Uint8 *stream, int len);

    static AudioSettings *m_AUDIO_SETTINGS;
    static VideoSettings *m_VIDEO_SETTINGS;
};

#endif
