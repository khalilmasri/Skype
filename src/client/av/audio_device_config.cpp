#include "audio_device_config.hpp"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_types.h>
#include <iostream>

AudioDevConfig *AudioDevConfig::m_instance = nullptr;

AudioDevConfig *AudioDevConfig::get_instance() {
  if (m_instance == nullptr) {
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

std::string AudioDevConfig::get_input() const { return m_selected_input; }
std::string AudioDevConfig::get_output() const { return m_selected_output; }

AudioDevConfig::DeviceListConst AudioDevConfig::list_input_name() const {
  return m_input_devs;
}
AudioDevConfig::DeviceListConst AudioDevConfig::list_output_name() const {
  return m_output_devs;
}

std::size_t AudioDevConfig::input_count() const { return m_input_devs.size(); }
std::size_t AudioDevConfig::output_count() const {
  return m_output_devs.size();
}

void AudioDevConfig::get_device_list() {

  int dev_count = SDL_GetNumAudioDevices(0);

  if (dev_count < 0) {
    std::cout << "Could not get number of devices\n";
    return;
  }

  for (int i = 0; i < dev_count; ++i) {

    const char *input_name = SDL_GetAudioDeviceName(i, m_INPUT_DEV);
    const char *output_name = SDL_GetAudioDeviceName(i, m_OUTPUT_DEV);
    if (input_name) {
      m_input_devs.push_back(input_name);
    }

    if (output_name) {
      m_output_devs.push_back(output_name);
    }
  }
}

void AudioDevConfig::select_device(std::size_t t_pos, DevType m_type) {
  bool valid = false;

  switch (m_type) {
  case Input:
    if (t_pos < m_input_devs.size()) {
      m_selected_input = m_input_devs.at(t_pos);
      valid = true;
    }
  case Output:
    if (t_pos < m_output_devs.size()) {
      m_selected_output = m_output_devs.at(t_pos);
      valid = true;
    }
  }

  if (!valid) {
    std::cout << "Error: Could not select input. Bad position.\n"; // TODO: Log
  }
}
