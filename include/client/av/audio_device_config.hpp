#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H
#include <string>
#include <vector>

class AudioDevConfig {

public:
  using DeviceListConst = const std::vector<std::string>&;

  AudioDevConfig(AudioDevConfig &lhs) = delete;
  void operator=(AudioDevConfig &lhs) = delete;

  static void delete_instance();
  static auto get_instance() -> AudioDevConfig*;

  void select_input(std::size_t t_pos);
  void select_output(std::size_t t_pos);

  [[nodiscard]] auto get_input() const -> std::string;
  [[nodiscard]] auto get_output() const -> std::string;

  [[nodiscard]] auto list_input_name() const -> DeviceListConst;
  [[nodiscard]] auto list_output_name() const -> DeviceListConst;

  [[nodiscard]] auto input_count() const -> std::size_t;
  [[nodiscard]] auto output_count() const -> std::size_t;

private:
  using DeviceList = std::vector<std::string> ;
  enum DevType { Input, Output };

  DeviceList m_input_devs;
  static AudioDevConfig *m_instance;
  DeviceList m_output_devs;

  std::string m_selected_input;
  std::string m_selected_output;

  const int m_INPUT_DEV = 1;
  const int m_OUTPUT_DEV = 0;

  void get_device_list();

  void select_device(std::size_t, DevType m_type);

  AudioDevConfig();
};

#endif // !AUDIO_CONFIG_H
