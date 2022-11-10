#include "call.hpp"
#include "av_data.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <string>
#include <unistd.h>

void Call::connect(Job &t_job) {

// This is temporary and just to illustrate we need to know from UI if there is video or not
  bool has_video = true;
  m_audio_p2p    = std::make_unique<P2P>(t_job.m_argument);
  bool valid     = udp_connect( m_audio_p2p, t_job); // omitting the t_wait_time argument so it's 1s by default

  // audio connection valid to stream
  if (valid) {
    audio_stream();
    audio_playback();
  }

  /* TODO(@Chris) we need to know from the UI if we have a video or not at this
   * point */
  /* initializing an peer to peer connection for the video  */
  if (has_video && valid) {
    m_video_p2p = std::make_unique<P2P>(t_job.m_argument);
    /* wait time between is much shorter; just 100 milliseconds as peer will most likely
     * be  already trying to accept the connection */
    valid = udp_connect(m_video_p2p, t_job, 100);
  }

  if (has_video && valid) {
    /* This is success connection. This is where we will call the video to
     * stream and playback. */
    LOG_DEBUG("second peer to peer connection was established succesfully");
  }
}

/* */

void Call::accept(Job &t_job) {
  // This is temporary and just to illustrate we need to know from UI if there is video or not
  bool has_video = true;
  m_audio_p2p    = std::make_unique<P2P>(t_job.m_argument);
  bool valid     = udp_accept(m_audio_p2p, t_job);


  // audio connection valid to stream
  if (valid) {
    audio_stream();
    audio_playback();
  }

  /* TODO(@Chris) we need to know from the UI if we have a video or not at this
   * point */
  /* initializing an peer to peer connection for the video  */
  if (has_video && valid) {
    m_video_p2p = std::make_unique<P2P>(t_job.m_argument);
    valid = udp_accept(m_video_p2p, t_job);

    int count = 0;

    /* if the accept fails we reset the p2p connection and try again until count == m_TIMEOUT */
    while (!valid) {
      m_video_p2p.reset();
      valid = udp_accept(m_video_p2p, t_job);

      /* will break after a certain number of trials */
      if (count >= m_TIMEOUT) {
        LOG_ERR("Accepting on video peer to peer connection has timed out.")
        break;
      }

      /* wait a bit of nothing to accept */
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      count++;
    }
  }

  if (has_video && valid) {
    /* This is success connection. This is where we will call the video to
       stream and playback. */
    LOG_DEBUG("second peer to peer connection was established succesfully");
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

  m_hangup = true;
  remove_caller(m_current);

  m_audio_stream.stop();
  m_playback.stop();

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

  m_audio_stream.start();
  m_audio_stream.stream(m_audio_p2p);
};

/* */

void Call::audio_playback() {

  /* NOTE: AVPlayback::buffer(conn, n);
   *       buffers 'n' number of data packages of AVdata before playback.
   *       A package has VideoSettings::m_capture_size
   *       frames of audio and video.
   */

  m_playback.buffer(m_audio_p2p, 1);
  m_playback.start(m_audio_p2p, m_audio_stream);
}

/* */

auto Call::udp_connect(P2PPtr &t_p2p_conn, Job &t_job, int t_wait_time)
    -> bool {

  std::string peer_id = std::to_string(t_job.m_intValue);
  t_p2p_conn->connect_peer(peer_id);

  if (m_audio_p2p->status() != P2P::Awaiting) {
    LOG_ERR("did not call connect correctly. Exiting...");
    return false;
  }

  int count = 0;

  while (m_audio_p2p->status() != P2P::Accepted) {

    LOG_INFO("Pinging...");

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
    LOG_INFO("Accepted was sucessful");
  } else {
    LOG_ERR("Accept failed.");
    return false;
  }

  t_p2p_conn->handshake_peer();
  return true;
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
