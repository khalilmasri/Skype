// #ifndef AV_DATA_H
// #define AV_DATA_H
// 
// #include "data.hpp"
// #include <cstdint>
// #include <vector>
// 
// class AVData : public Data {
// 
//     ~AVData() override;
// 
//      [[nodiscard]] auto   get_type() const -> Data::type override;
//      [[nodiscard]] auto   get_data() const -> DataVector override;
//      [[nodiscard]] auto   empty() const -> bool override;
//      [[nodiscard]] auto   size() const -> std::size_t override;
// 
//      std::vector<uint8_t> m_data;
//      std::size_t          m_size;
//      const Data::type     m_type = Data::AudioVideo;
// };
// 
// #endif
