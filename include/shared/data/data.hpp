#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <string>
#include <vector>

class Data {

public:
  enum type { Audio = 0, Video = 1, Text = 2, Empty = 3 };
  using DataVector = std::vector<uint8_t>;

  [[nodiscard]] virtual auto get_type() const -> type = 0;
  [[nodiscard]] virtual auto get_data() const -> DataVector = 0;
  [[nodiscard]] virtual auto empty() const -> bool = 0;
  [[nodiscard]] virtual auto size() const -> std::size_t = 0;

  virtual ~Data() = default;

  static std::string type_to_string(type t_type) {
    switch (t_type) {
    case Audio:
      return "Audio";
    case Video:
      return "Video";
    case Text:
      return "Text";
    case Empty:
      return "Empty";
  }
  }
};

#endif // !DATA_H
