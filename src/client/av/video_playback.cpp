#include "video_playback.hpp"

#include <fstream>

VideoPlayback::VideoPlayback(Webcam &t_video_playback)
               : m_webcam(t_video_playback),
               m_queue(std::make_shared<Webcam::CVMatQueue>()){ }

void VideoPlayback::start(P2PPtr &t_p2p_conn, std::function<void()> t_hangup_callback) {

  if (m_status == Stopped) {
    m_done_received = false;
    m_status        = Started;
    spawn_network_read_thread(t_p2p_conn, t_hangup_callback);
  //  spawn_video_playback_thread(); //not running this from this thread. Instead from from main 

  } else {
    LOG_ERR("Could not START audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}

void VideoPlayback::stop() {

  if (m_status == Started) {

    //  flush queue
    while (!m_queue->empty()) { 
       cv::Mat mat;
       m_queue->pop_try(mat);
    }

    m_status = Stopped;
    LOG_INFO("Closing Video Playback...");

    if(m_webcam.status() == Webcam::Ready){
      LOG_INFO("VideoPlayback releasing webcam...");
      m_webcam.release();
    }

  } else {
    LOG_ERR("Could not STOP Audio Playback because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void VideoPlayback::load(const Data *t_video_data) {

  if (valid_data_type(t_video_data, Data::Video)) {
    const Webcam::WebcamFrame frame = t_video_data->get_data();
    LOG_TRACE("Loading frame size %lu", frame.size());
    // NOT DECODE AS cv::Mat and decode as a QT format or just push the jpeg.
    m_webcam.decode_one(frame, m_queue); // converts and pushes to video queue.
  }

}

auto VideoPlayback::get_stream() -> VideoQueuePtr { return m_queue; }


