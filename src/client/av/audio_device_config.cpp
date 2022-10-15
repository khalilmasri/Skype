#include "audio_device_config.hpp"
#include "logger.hpp"
#include <SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_types.h>
#include <iostream>

AudioDevConfig *AudioDevConfig::m_instance = nullptr;

auto AudioDevConfig::get_instance() -> AudioDevConfig * {
  if (m_instance == nullptr) {

    Uint32 subsystem_init = 0;

    if (SDL_WasInit(subsystem_init & SDL_INIT_AUDIO) <= 0) {
      SDL_Init(SDL_INIT_AUDIO);
    }

    LOG_INFO("Init audio config!");
    m_instance = new AudioDevConfig();
  }

  return m_instance;
}

void AudioDevConfig::delete_instance() { delete m_instance; }

AudioDevConfig::AudioDevConfig() {
  get_device_list();
  select_device(0, Input);
  select_device(0, Output);
}

void AudioDevConfig::select_input(std::size_t t_pos) {
  select_device(t_pos, Input);
}
void AudioDevConfig::select_output(std::size_t t_pos) {
  select_device(t_pos, Output);
}

auto AudioDevConfig::get_input() const -> std::string {
  return m_selected_input;
}
auto AudioDevConfig::get_output() const -> std::string {
  return m_selected_output;
}

auto AudioDevConfig::list_input_name() const
    -> AudioDevConfig::DeviceListConst {
  return m_input_devs;
}
auto AudioDevConfig::list_output_name() const
    -> AudioDevConfig::DeviceListConst {
  return m_output_devs;
}

auto AudioDevConfig::input_count() const -> std::size_t {
  return m_input_devs.size();
}
auto AudioDevConfig::output_count() const -> std::size_t {
  return m_output_devs.size();
}

void AudioDevConfig::get_device_list() {

  int dev_count = SDL_GetNumAudioDevices(0);

  if (dev_count < 0) {
    LOG_ERR("Could not get number of devices");
    return;
  }

  for (int i = 0; i < dev_count; ++i) {

    const char *input_name = SDL_GetAudioDeviceName(i, m_INPUT_DEV);
    const char *output_name = SDL_GetAudioDeviceName(i, m_OUTPUT_DEV);
    if (static_cast<bool>(input_name)) {
      m_input_devs.push_back(input_name);
    }

    if (static_cast<bool>(output_name)) {
      m_output_devs.push_back(output_name);
    }
  }
}

void AudioDevConfig::select_device(std::size_t t_pos, DevType m_type) {
  switch (m_type) {
  case Input:
    if (t_pos < m_input_devs.size()) {
      m_selected_input = m_input_devs.at(t_pos);
    } else {
      LOG_DEBUG("Could not select input device at position %d. Only %llu are "
                "available",
                t_pos, m_input_devs.size())
    }

  case Output:
    if (t_pos < m_output_devs.size()) {
      m_selected_output = m_output_devs.at(t_pos);
    } else {
      LOG_DEBUG("Could not select output device at position %d. Only %llu are "
                "available",
                t_pos, m_output_devs.size())
    }
  }
}
