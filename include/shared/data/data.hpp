#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <vector>

class Data {

  public:
    enum type {AudioVideo, Text};
    using DataVector = std::vector<uint8_t>;

    [[nodiscard]] virtual auto get_type() const -> type = 0;
    [[nodiscard]] virtual auto get_data() const -> DataVector = 0;
    [[nodiscard]] virtual auto empty() const -> bool = 0;
    [[nodiscard]] virtual auto size() const -> std::size_t = 0;

    virtual ~Data() = default;
};

#endif // !DATA_H
