#include "call.hpp"
#include "av_data.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <string>
#include <unistd.h>

void Call::connect(Job &t_job) {
  m_audio_p2p = std::make_unique<P2P>(t_job.m_argument);

  std::string peer_id = std::to_string(t_job.m_intValue);
  m_audio_p2p->connect_peer(peer_id);

  LOG_ERR("did not call connect correctly. Exiting...");
  if (m_audio_p2p->status() != P2P::Awaiting) {
    return;
  }

  int count = 0;

  while (m_audio_p2p->status() != P2P::Accepted) {

    LOG_INFO("Pinging...");

    m_audio_p2p->ping_peer();
    sleep(1); // check every 1 second

    if (m_audio_p2p->status() == P2P::Awaiting) {
      LOG_DEBUG("Still Awaiting...");
    }

    if (m_audio_p2p->status() == P2P::Error) {
      LOG_ERR("ping returned an error. exiting....");
      remove_caller(t_job.m_intValue);
      return;
    }

    if (m_hangup) {
      LOG_DEBUG("Hanging up the call!");
      m_audio_p2p->hangup_peer();
      m_hangup = false;     
      return;
    }

    if (count > m_TIMEOUT) {
      LOG_INFO("Breaking after %d seconds", count);
      JobBus::create({Job::HANGUP}); // TODO(@khalil): is this correct?
      m_audio_p2p->hangup_peer();
      return;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  m_audio_p2p->handshake_peer();

    av_stream();
    av_playback();
}

/* */

void Call::accept(Job &t_job) {

  //  P2P call(t_job.m_argument);
  m_audio_p2p = std::make_unique<P2P>(t_job.m_argument);

  std::string user_id = std::to_string(t_job.m_intValue);
  m_audio_p2p->accept_peer(user_id);
  LOG_INFO("Accepting call from %d", t_job.m_intValue);

  if (m_audio_p2p->status() == P2P::Accepted) {
    LOG_INFO("Accepted was sucessful");
  } else {
    LOG_ERR("Accept failed.");
    return;
  }

  LOG_INFO("Call accepted");
  m_audio_p2p->handshake_peer();

  av_stream();
  av_playback();
}

/* */

void Call::reject(Job &t_job) {

 if (nullptr != m_audio_p2p) {
    std::string user_id = std::to_string(t_job.m_intValue);
    m_audio_p2p->reject_peer(user_id);

  } else {
    LOG_ERR("There are no existing calls to reject.");
  }
}

/* */

void Call::awaiting(Job &t_job) {
  
  if ( false == t_job.m_boolValue ) {

    LOG_INFO("Awaiting call ended");
    remove_caller(t_job.m_intValue);
    t_job.m_valid = true;
    return;
  }

  t_job.m_valid = false; 

  auto it = std::find(m_callers.begin(), m_callers.end(), t_job.m_intValue);
  
  LOG_INFO("Getting the call");
  
  if ( m_callers.end() == it ) {
    LOG_INFO("Getting the call was success");
    m_callers.append(t_job.m_intValue);
    t_job.m_valid = true;
  }
}

/* */


void Call::hangup() {
  m_hangup = true;
  
  LOG_DEBUG("Hanging up and closing the connection!");
  remove_caller(m_current);
  
  m_stream.stop();
  m_playback.stop();

  m_current = -1;
}

/* */

void Call::remove_caller(int t_caller) {
  try{
    m_callers.removeOne(t_caller);
  }catch(...){
    // TODO(@khalil): Log error?
    LOG_ERR("Couldn't remove caller!!!");
  };
}

/* */

void Call::av_stream() {

  /* create the callback for DataStream */
  AVStream::StreamCallback callback  = stream_callback();

  if(callback == nullptr){
    std::cout << "callback is nullptr\n";
  }

  m_stream.start();
  m_stream.stream(std::move(callback));
};

/* */

void Call::av_playback(){

  /* NOTE: AVPlayback::buffer(conn, n);
   *       buffers 'n' number of data packages of AVdata before playback.
   *       A package has VideoSettings::m_capture_size
   *       frames of audio and video.
   */

  m_playback.buffer(m_audio_p2p, 1);
  m_playback.start(m_audio_p2p);
}

void Call::webcam() {
  if (!m_webcam) {
    LOG_INFO("Turning webcam");
    m_webcam = true;
  } else {
    LOG_INFO("Closing webcam");
    m_webcam = false;
  }
}

auto Call::stream_callback() -> AVStream::StreamCallback {

  /*  make_request will created a request with peer information based on
   *  UDP connection established by m_call(P2P) */

  return [this](Data::DataVector &&t_audio) {

    Request audio_req = m_audio_p2p->make_request();

    audio_req.set_data(new AVData(std::move(t_audio), Data::Audio));
    m_audio_p2p->send_package(audio_req);
  };
}
