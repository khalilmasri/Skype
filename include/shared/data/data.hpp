#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <string>
#include <vector>

class Data {

public:
  enum Type { Audio = 1, Video = 2, Text = 3, Empty = 4, Done = 5 };
  using DataVector = std::vector<uint8_t>;

  [[nodiscard]] virtual auto get_type() const -> Type = 0;
  [[nodiscard]] virtual auto get_data() const -> DataVector = 0;
  [[nodiscard]] virtual auto get_data_ref () const -> const DataVector& = 0;
  [[nodiscard]] virtual auto empty() const -> bool = 0;
  [[nodiscard]] virtual auto size() const -> std::size_t = 0;

  virtual ~Data() = default;

  static std::string type_to_string(Type t_type) {
    switch (t_type) {
    case Audio:
      return "Audio";
    case Video:
      return "Video";
    case Text:
      return "Text";
    case Empty:
      return "Empty";
    case Done :
      return "Done";
    }
  }

  static uint8_t type_to_char(Type t_type) {
    switch (t_type) {
    case Audio:
      return 'a';
    case Video:
      return 'v';
    case Text:
      return 't';
    case Empty:
      return 'e';
    case Done :
      return 'd';

    }
  }

  static Data::Type char_to_type(uint8_t c) {
    switch (c) {
    case 'a':
      return Audio;
    case 'v':
      return Video;
    case 't':
      return Text;
    case 'd':
      return Done;
    default:
      return Empty;
    }
  }
};

#endif // !DATA_H
