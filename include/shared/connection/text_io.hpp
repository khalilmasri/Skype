#ifndef TEXT_MESSAGE_H
#define TEXT_MESSAGE_H
#include "IO_strategy.hpp"

class TextIO : public IOStrategy {

public:
  bool receive(int t_socket, std::string &t_data) const override;
  bool respond(int t_socket, std::string &t_data) const override;

private:
  int  read_header(int t_socket) const;
  bool is_valid(int t_result, const char *t_msg) const;

  ~TextIO(){};

};

#endif // !TEXT_MESSAGE_H
