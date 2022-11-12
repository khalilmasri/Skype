#include "av_data.hpp"
#include <iostream>

// passing not data creates an empty Data.
AVData::AVData() : m_type(Data::Empty){};

AVData::AVData(Data::Type t_type) :  m_type(t_type){};

AVData::AVData(Data::DataVector &&t_data, Data::Type t_type) : m_data(std::move(t_data)), m_type(t_type) {
}

auto AVData::get_data() const -> Data::DataVector { return m_data; }
auto AVData::get_data_ref() const -> const Data::DataVector& { return m_data; }
auto AVData::get_type() const -> Data::Type  { return m_type; }
auto AVData::empty() const -> bool  { return m_data.empty(); }
auto AVData::size() const -> std::size_t  { return m_data.size(); }

AVData::~AVData() = default;
