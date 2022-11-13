#include "call.hpp"
#include "av_data.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <string>
#include <unistd.h>

Call::Call() : m_audio_stream(m_webcam),
               m_video_stream(m_webcam),
               m_video_playback(m_webcam){

  m_audio_stream.set_stream_type(AVStream::Audio);
  m_video_stream.set_stream_type(AVStream::Video);
}

void Call::connect(Job &t_job) {

// This is temporary and just to illustrate we need to know from UI if there is video or not
  m_token = t_job.m_argument;
  std::thread call_thread([&t_job, this](){
    bool has_video = true;

    m_audio_p2p = nullptr;
    m_video_p2p = nullptr;

    m_audio_p2p    = std::make_unique<P2P>(m_token);
    bool valid     = udp_connect( m_audio_p2p, t_job); // omitting the t_wait_time argument so it's 1s by default

    /* TODO(@Chris) we need to know from the UI if we have a video or not at this
    * point */
    /* initializing an peer to peer connection for the video  */
    if (has_video && valid) {
      m_video_p2p = std::make_unique<P2P>(m_token);
      /* wait time between is much shorter; just 100 milliseconds as peer will most likely
      * be  already trying to accept the connection */
      valid = udp_connect(m_video_p2p, t_job, 300);
    }

    if (has_video && valid) {
      LOG_DEBUG("Starting Call::connect Video.");
      m_webcam.init();
      // video_stream();
      video_playback();

      // send job back to the UI to display frames
      t_job.m_command = Job::VIDEO_STREAM;
      // set jobs with a shared pointer to access the video data from UI
      t_job.m_video_stream = m_video_playback.get_stream();
    }

    if (valid) {
      LOG_DEBUG("Starting Call::connect Audio.");
      audio_stream();
      audio_playback();
    }

  });
  
  call_thread.detach();
}

/* */

void Call::accept(Job &t_job) {
  // This is temporary and just to illustrate we need to know from UI if there is video or not
  bool has_video = true;

  m_audio_p2p = nullptr;
  m_video_p2p = nullptr;

  m_audio_p2p    = std::make_unique<P2P>(t_job.m_argument);
  bool valid     = udp_accept(m_audio_p2p, t_job);


  /* TODO(@Chris) we need to know from the UI if we have a video or not at this
   * point */
  /* initializing an peer to peer connection for the video  */
  if (has_video && valid) {
    m_video_p2p = std::make_unique<P2P>(t_job.m_argument);
    valid = udp_accept(m_video_p2p, t_job);

    int count = 0;

    /* if the accept fails we reset the p2p connection and try again until count == m_TIMEOUT */
    while (!valid) {
      m_video_p2p->reset();
      valid = udp_accept(m_video_p2p, t_job);

      /* will break after a certain number of trials */
      if (count >= m_TIMEOUT + 50) {
        LOG_ERR("Accepting on video peer to peer connection has timed out.")
        break;
      }

      /* wait a bit of nothing to accept */
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      count++;
    }
  }

if (has_video && valid) {
    LOG_DEBUG("Starting Call::accept Video.");
      m_webcam.init();
      video_stream();
      //video_playback();
  }

  if (valid) {
    LOG_DEBUG("Starting Call::accept Audio.");
    audio_stream();
    audio_playback();
  }

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

  if (false == t_job.m_boolValue) {
    LOG_INFO("Awaiting call ended");
    remove_caller(t_job.m_intValue);
    t_job.m_valid = true;
    return;
  }

  t_job.m_valid = false;

  auto it = std::find(m_callers.begin(), m_callers.end(), t_job.m_intValue);

  LOG_INFO("Getting the call");

  if (m_callers.end() == it) {
    LOG_INFO("Finding the call was success");
    m_callers.append(t_job.m_intValue);
    t_job.m_valid = true;
  }
}

/* */

void Call::hangup() {

  LOG_INFO("Hanging up call.");
  m_hangup = true;
  remove_caller(m_current);

  m_audio_stream.stop();
  m_audio_playback.stop();
  //m_video_playback.stop();
  m_video_stream.stop();

  m_current = -1;
}

/* */

void Call::remove_caller(int t_caller) {
  try {
    m_callers.removeOne(t_caller);
  } catch (...) {
    LOG_ERR("Could not remove called.")
  };
}

/* */

void Call::audio_stream() {
  m_audio_stream.set_stream_type(AVStream::Audio);
  m_audio_stream.start();
  m_audio_stream.stream(m_audio_p2p);
  
};

/* */

void Call::audio_playback() {

  /* NOTE: AVPlayback::buffer(conn, n);
   *       buffers 'n' number of data packages of AVdata before playback.
   *       frames of audio and video.
   */

  m_audio_playback.buffer(m_audio_p2p, m_NB_BUFFER_PACKETS);
  m_audio_playback.start(m_audio_p2p, m_audio_stream);
}

/* */

void Call::video_stream(){
  m_video_stream.set_stream_type(AVStream::Video);
  m_video_stream.start();
  m_video_stream.stream(m_video_p2p);
}


/* */

void Call::video_playback(){
  m_video_playback.buffer(m_video_p2p, m_NB_BUFFER_PACKETS);
  m_video_playback.start(m_video_p2p, m_video_stream);
}


/* */

auto Call::udp_connect(P2PPtr &t_p2p_conn, Job &t_job, int t_wait_time)
    -> bool {

  std::string peer_id = std::to_string(t_job.m_intValue);
  t_p2p_conn->connect_peer(peer_id);

  if (t_p2p_conn->status() != P2P::Awaiting) {
    LOG_ERR("did not call connect correctly. Exiting...");
    return false;
  }

  int count = 0;

  while (t_p2p_conn->status() != P2P::Accepted) {

    LOG_INFO("Pinging   ...");

    t_p2p_conn->ping_peer();

    // default wait time is 1 second.
    // When hole punching a second udp connection we can go much quicker.
    std::this_thread::sleep_for(std::chrono::milliseconds(t_wait_time));

    if (t_p2p_conn->status() == P2P::Awaiting) {
      LOG_DEBUG("Awaiting peer...");
    }

    if (t_p2p_conn->status() == P2P::Error) {
      LOG_ERR("Ping has returned an error. exiting....");
      remove_caller(t_job.m_intValue);
      return false;
    }

    if (m_hangup) {
      LOG_DEBUG("Hanging up the call requested by initiator.");
      t_p2p_conn->hangup_peer();
      m_hangup = false;
      return m_hangup;
    }

    if (count > m_TIMEOUT) {
      LOG_INFO("Breaking after %d seconds", count);
      JobBus::create({Job::HANGUP}); // TODO(@khalil): is this correct?
      t_p2p_conn->hangup_peer();
      return false;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  t_p2p_conn->handshake_peer();

  return true;
}

auto Call::udp_accept(P2PPtr &t_p2p_conn, Job &t_job) -> bool {

  std::string user_id = std::to_string(t_job.m_intValue);

  t_p2p_conn->accept_peer(user_id);
  LOG_INFO("Attempting to accept call from %d", t_job.m_intValue);

  if (t_p2p_conn->status() == P2P::Accepted) {
    LOG_INFO("Accept was sucessfull.");

  } else {

    LOG_ERR("Accept failed.");
    return false;
  }

  t_p2p_conn->handshake_peer();
  return true;
}

