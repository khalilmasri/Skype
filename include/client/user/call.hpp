#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"
#include "peer_to_peer.hpp"

#include <QVector>
#include <QString>

class Call{

public:
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void webcam();
  void mute();
  void hangup();

private:
  bool m_hangup = false;
  bool m_webcam = false;
  bool m_mute = false;
};

#endif // CALL_H
