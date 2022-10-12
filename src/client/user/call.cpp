#include "call.hpp"
#include "av_data.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <string>
#include <unistd.h>

void Call::connect(Job &t_job) {
  m_p2pconn = std::make_unique<P2P>(t_job.m_argument);

  std::string peer_id = std::to_string(t_job.m_intValue);
  m_p2pconn->connect_peer(peer_id);

  LOG_ERR("did not call connect correctly. Exiting...");
  return;
  if (m_p2pconn->status() != P2P::Awaiting) {
  }

  int count = 0;

  while (m_p2pconn->status() != P2P::Accepted) {

    LOG_INFO("Pinging...");

    m_p2pconn->ping_peer();
    sleep(1); // check every 1 second

    if (m_p2pconn->status() == P2P::Awaiting) {
      LOG_DEBUG("Still Awaiting...");
    }

    if (m_p2pconn->status() == P2P::Error) {
      LOG_ERR("ping returned an error. exiting....");
      remove_caller(t_job.m_intValue);
      return;
    }

    if (m_hangup) {
      LOG_DEBUG("Hanging up the call!");
      m_p2pconn->hangup_peer();
      m_hangup = false;     
      return;
    }

    if (count > m_TIMEOUT) {
      LOG_INFO("Breaking after %d seconds", count);
      JobBus::create({Job::HANGUP}); // TODO(@khalil): is this correct?
      m_p2pconn->hangup_peer();
      return;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  m_p2pconn->handshake_peer();
}

/* */

void Call::accept(Job &t_job) {

  //  P2P call(t_job.m_argument);
  m_p2pconn = std::make_unique<P2P>(t_job.m_argument);

  std::string user_id = std::to_string(t_job.m_intValue);
  m_p2pconn->accept_peer(user_id);

  if (m_p2pconn->status() == P2P::Accepted) {
    LOG_INFO("Accepted was sucessful");
  } else {
    LOG_ERR("Accept failed.");
    return;
  }

  LOG_INFO("Call accepted");
  m_p2pconn->handshake_peer();
}

/* */

void Call::reject(Job &t_job) {

 if (nullptr != m_p2pconn) {
    std::string user_id = std::to_string(t_job.m_intValue);
    m_p2pconn->reject_peer(user_id);

  } else {
    LOG_ERR("There are no existing calls to reject.");
  }
}

/* */

void Call::awaiting(Job &t_job)
{
  
  if ( false == t_job.m_boolValue ) {
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
  
  remove_caller(m_current);

  m_current = -1;
}

/* */

void Call::remove_caller(int t_caller) {
  try{
    m_callers.removeOne(t_caller);
  }catch(...){
    // TODO(@khalil): Log error?

  };
}

/* */

void Call::av_stream() {

  /* create the callback for DataStream */
  AVStream::DataCallback callback  = data_callback();

  m_stream.start();
  m_stream.stream(callback);
};

/* */

void Call::av_playback(){
  // buffers n data packages
  // of av data (each packages has VideoSettings::m_capture_size
  // frames of audio and video)
  m_playback.buffer(m_p2pconn, 5);
  m_playback.start(m_p2pconn);
}


/* */

void Call::mute() {
  if (!m_mute) {
    LOG_INFO("Mute ON");
    m_mute = true;
  } else {
    LOG_INFO("Mute OFF");
    m_mute = false;
  }
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

auto Call::data_callback() -> AVStream::DataCallback {

  /*  make_request will created a request with peer information based on
   *  UDP connection established by m_call(P2P) */

  Request audio_req = m_p2pconn->make_request();
  Request video_req = m_p2pconn->make_request();


  return [this, &audio_req, &video_req](Webcam::WebcamFrames &&t_video,
                                        Data::DataVector &&t_audio) {
    // send video first
    for (Data::DataVector &frame_data : t_video) {
      video_req.set_data(new AVData(std::move(frame_data), Data::Video));
      m_p2pconn->send_package(video_req);
    }

    //  send audio
    audio_req.set_data(new AVData(std::move(t_audio), Data::Audio));
    m_p2pconn->send_package(audio_req);
  };
}
