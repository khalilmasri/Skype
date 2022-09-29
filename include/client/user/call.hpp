#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"
#include "peer_to_peer.hpp"

#include <QVector>
#include <QString>
#include <cstdint>

class Call{

public:
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void awaiting(Job &t_job);
  void remove_caller(int t_caller);
  void webcam();
  void mute();
  void hangup();

private:
  QVector<int> m_callers;
  int          m_current;
  bool m_hangup = false;
  bool m_webcam = false;
  bool m_mute = false;
};

#endif // CALL_H
