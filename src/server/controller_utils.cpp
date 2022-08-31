#include "controller_utils.hpp"
#include "text_data.hpp"

void ControllerUtils::set_request_reply(Reply::Code t_reply, Request &t_req) {
  std::string reply = Reply::get_message(t_reply);
  t_req.set_data(new TextData(reply));
}

void ControllerUtils::set_request_reply(Reply::Code t_reply, std::string &&t_msg,
                                    Request &t_req) {
  std::string reply = Reply::append_message(t_reply, t_msg);
  t_req.set_data(new TextData(reply));
}

void ControllerUtils::set_request_reply(bool t_valid, Request &t_req) {
  if (t_valid) {
    set_request_reply(Reply::r_200, t_req);

  } else {
    set_request_reply(Reply::r_500, t_req);
  }
}

void ControllerUtils::set_request_reply(bool t_valid, std::string &&t_msg, Request &t_req) {
  if (t_valid) {
    set_request_reply(Reply::r_201, std::move(t_msg), t_req);

  } else {
    set_request_reply(Reply::r_500, t_req);
  }
}


