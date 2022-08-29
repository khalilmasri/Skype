#ifndef CHAT_H
#define CHAT_H

#include "user.hpp"
#include "request.hpp"
#include "user_chat.hpp"
#include "active_conn.hpp"

#include <QVector>
#include <QString>

class Chat : public UserChat {

public:
  
  QVector<Chat> get_pending(ActiveConn& t_conn, Request &t_req);
  QVector<Chat> get_all(ActiveConn& t_conn, Request &t_req);
  bool send(ActiveConn& t_conn, Request &t_req);
  bool deliver(ActiveConn& t_conn, Request &t_req);

private:
  QVector<Chat> parse_chat(std::string &t_response);
  bool valid_response(Reply::Code t_code, std::string& t_res);

};

#endif
