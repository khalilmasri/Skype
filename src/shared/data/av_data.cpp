#include "av_data.hpp"

AVData::AVData(Data::DataVector &&t_data, Data::type t_type) : m_data(t_data), m_type(t_type) {}

auto AVData::get_data() const -> Data::DataVector { return m_data; }
auto AVData::get_type() const -> Data::type  { return m_type; }
auto AVData::empty() const -> bool  { return m_data.empty(); }
auto AVData::size() const -> std::size_t  { return m_data.size(); }

AVData::~AVData() = default;
