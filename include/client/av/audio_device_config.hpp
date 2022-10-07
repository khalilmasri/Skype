#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H
#include <string>
#include <vector>

class AudioDevConfig {

public:
  typedef const std::vector<std::string> &DeviceListConst;
  AudioDevConfig(AudioDevConfig &lhs) = delete;
  void operator=(AudioDevConfig &lhs) = delete;

  static void delete_instance();
  static AudioDevConfig *get_instance();

  void select_input(std::size_t t_pos);
  void select_output(std::size_t t_pos);

  std::string get_input() const;
  std::string get_output() const;

  DeviceListConst list_input_name() const;
  DeviceListConst list_output_name() const;

  std::size_t input_count() const;
  std::size_t output_count() const;

private:
  typedef std::vector<std::string> DeviceList;
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
