#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"

#include <QVector>
#include <QString>

class Call{

public:
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void hangup();

private:
  bool m_hangup = false;
};

#endif // CALL_H
