#include "call.hpp"
#include "av_data.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <string>

void Call::connect(Job &t_job) {
  m_call = std::make_unique<P2P>(t_job.m_argument);

  std::string peer_id = std::to_string(t_job.m_intValue);
  m_call->connect_peer(peer_id);

  LOG_ERR("did not call connect correctly. Exiting...");
  return;
  if (m_call->status() != P2P::Awaiting) {
  }

  int count = 0;

  while (m_call->status() != P2P::Accepted) {

    LOG_INFO("Pinging...");

    m_call->ping_peer();
    sleep(1); // check every 1 second

    if (m_call->status() == P2P::Awaiting) {
      LOG_DEBUG("Still Awaiting...");
    }

    if (m_call->status() == P2P::Error) {
      LOG_ERR("ping returned an error. exiting....");
      return;
    }

    if (m_hangup) {
      LOG_DEBUG("Hanging up the call!");
      m_hangup = false;
      return;
    }

    if (count > m_TIMEOUT) {
      LOG_INFO("Breaking after %d seconds", count);
      JobBus::create({Job::HANGUP}); // TODO(@khalil): is this correct?
      m_call->hangup_peer();
      return;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  m_call->handshake_peer();
  // the call will make a ready to go request with peer address.
}

void Call::accept(Job &t_job) {

  //  P2P call(t_job.m_argument);
  m_call = std::make_unique<P2P>(t_job.m_argument);

  std::string user_id = std::to_string(t_job.m_intValue);
  m_call->accept_peer(user_id);

  if (m_call->status() == P2P::Accepted) {
    LOG_INFO("Accepted was sucessful");
  } else {
    LOG_ERR("Accept failed.");
  }

  LOG_INFO("Call accepted");
  m_call->handshake_peer();
}

void Call::reject(Job &t_job) {

  if (nullptr != m_call) {
    std::string user_id = std::to_string(t_job.m_intValue);
    m_call->reject_peer(user_id);

  } else {
    LOG_ERR("There are no existing calls to reject.");
  }
}

void Call::stream() {

  /* create the callback for DataStream */
  AVStream::DataCallback callback  = data_callback();

  m_stream.start();
  m_stream.stream(callback);
};

void Call::hangup() { m_hangup = true; }

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

  Request audio_req = m_call->make_request();
  Request video_req = m_call->make_request();


  return [this, &audio_req, &video_req](Webcam::WebcamFrames &&t_video,
                                        Data::DataVector &&t_audio) {
    // send video first
    for (Data::DataVector &frame_data : t_video) {
      video_req.set_data(new AVData(std::move(frame_data), Data::Video));
      m_call->send_package(video_req);
    }

    //  send audio
    audio_req.set_data(new AVData(std::move(t_audio), Data::Audio));
    m_call->send_package(audio_req);
  };
}
