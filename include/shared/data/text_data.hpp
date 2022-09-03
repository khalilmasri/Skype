#ifndef CONTROL_DATA_H
#define CONTROL_DATA_H

#include "data.hpp"
#include "reply.hpp"
#include "server_commands.hpp"

class TextData : public Data {

  public:
     TextData(std::string &t_text);
     TextData(std::string &&t_text);
     TextData(const char* t_text);
     TextData(char* t_text);

    ~TextData() override;

     Data::type           get_type() const override;
     std::byte*           get_data() const override;
     bool                 empty() const override;
     std::size_t          size() const override;


     static              std::string to_string(const Data* t_data);

  private:
   std::byte             *m_data;
   std::size_t            m_size;

   const Data::type       m_type = Data::Text;
   void                   load_text(std::string &t_text);

#endif // !CONTROL_DATA_H
};
