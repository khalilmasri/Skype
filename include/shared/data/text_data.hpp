#ifndef CONTROL_DATA_H
#define CONTROL_DATA_H

#include "data.hpp"
#include "reply.hpp"
#include "server_commands.hpp"
#include <vector>

class TextData : public Data {

  public:
     explicit TextData(std::string &t_text);
     explicit TextData(const char* t_text);
     explicit TextData(char* t_text);
     explicit TextData(std::string &&t_text);

    ~TextData() override;

     [[nodiscard]] auto  get_type() const -> Data::Type override;
     [[nodiscard]] auto  get_data() const -> DataVector override;
     [[nodiscard]] auto  get_data_ref() const -> const DataVector& override;
     [[nodiscard]] auto  empty() const -> bool override;
     [[nodiscard]] auto  size() const -> std::size_t override;


     static        auto to_string(const Data* t_data) -> std::string;

  private:
   std::vector<uint8_t>   m_data;
   const Data::Type       m_type = Data::Text;

   void                   load_text(std::string &t_text);

#endif // !CONTROL_DATA_H
};
