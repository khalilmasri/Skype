#include "video_playback.hpp"

VideoPlayback::VideoPlayback() {
  m_type = Video;
  m_webcam.init();
}

void VideoPlayback::start(P2PPtr &t_p2p_conn, AVStream &t_stream) {

  if (m_status == Stopped) {
    m_done_received = false;
    m_status = Started;
    spawn_network_read_thread(t_p2p_conn, t_stream);
    spawn_video_playback_thread();

  } else {
    LOG_ERR("Could not START audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}

void VideoPlayback::stop() {

  if (m_status == Started) {

    // wait for the queue to be empty before stopping.
    while (!m_queue.empty()) {
      Webcam::wait();
    }

    m_status = Stopped;
    LOG_INFO("Closing Video Playback...");

  } else {
    LOG_ERR("Could not STOP Audio Playback because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void VideoPlayback::load(const Data *t_video_data) {

  if (valid_data_type(t_video_data, Data::Video)) {

    Webcam::WebcamFrame frame = t_video_data->get_data();
    m_webcam.decode_one(frame, m_queue); // converts and pushes to video queue.
  }
}

void VideoPlayback::spawn_video_playback_thread() {

  std::thread video_playback_thread([this]() {
    while (m_status == Started) {

      if (m_queue.empty()) {
        LOG_TRACE("Webcam queue is empty. Trying again...")
        continue;
      }

      Webcam::WebcamMat mat;

      bool valid = m_queue.pop_try(mat);

      if (valid) {
        Webcam::show(mat);
      }

      Webcam::wait();
    }
  });
}