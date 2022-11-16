#ifndef AV_DATA_H
#define AV_DATA_H

#include "data.hpp"
#include <cstdint>
#include <vector>

class AVData : public Data {

public:
  explicit AVData(Data::DataVector &&t_data, Data::Type t_type);
  explicit AVData(Data::Type t_type);
  explicit AVData();
  ~AVData() override;

  [[nodiscard]] auto get_type() const -> Data::Type override;
  [[nodiscard]] auto get_data() const -> Data::DataVector override;
  [[nodiscard]] auto get_data_ref() const -> const DataVector& override;
  [[nodiscard]] auto empty() const -> bool override;
  [[nodiscard]] auto size() const -> std::size_t override;

private:
  Data::DataVector m_data;
  const Data::Type m_type;
};

#endif
