#ifndef AV_DATA_H
#define AV_DATA_H

#include "data.hpp"
#include <cstdint>
#include <vector>

class AVData : public Data {

public:
  explicit AVData(Data::DataVector &&t_data, Data::type t_type);
  ~AVData() override;

  [[nodiscard]] auto get_type() const -> Data::type override;
  [[nodiscard]] auto get_data() const -> Data::DataVector override;
  [[nodiscard]] auto empty() const -> bool override;
  [[nodiscard]] auto size() const -> std::size_t override;

private:
  Data::DataVector m_data;
  const Data::type m_type;
};

#endif
