#ifndef TEXT_MESSAGE_H
#define TEXT_MESSAGE_H
#include "IO_strategy.hpp"
#include "request.hpp"

class TextIO : public IOStrategy {

public:
  bool         receive(Request &t_req) const override;
  bool         respond(Request &t_req) const override;

private:
  int          read_header(int t_socket) const;
  std::string  create_header(int t_msg_length) const;

  ~TextIO(){};

};

#endif // !TEXT_MESSAGE_H
