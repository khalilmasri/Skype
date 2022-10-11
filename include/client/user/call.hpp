#ifndef CALL_H
#define CALL_H

#include "user.hpp"
#include "request.hpp"
#include "active_conn.hpp"
#include "job.hpp"
#include "peer_to_peer.hpp"
#include "av_stream.hpp"

#include <QVector>
#include <QString>
#include <memory>

class Call{

public:
  void create(Job &t_job);
  void connect(Job &t_job);
  void accept(Job &t_job);
  void reject(Job &t_job);
  void stream();
  void webcam();
  void mute();
  void hangup();

private:
  using P2PPtr = std::unique_ptr<P2P>;

  bool     m_hangup = false;
  bool     m_webcam = false;
  bool     m_mute   = false;
  P2PPtr   m_call   = nullptr;
  AVStream m_stream;

 auto data_callback() -> AVStream::DataCallback;

  inline static const int m_TIMEOUT = 10;
};

#endif // CALL_H
