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

    // wait for the queue to be empty before stopping.
    while (!m_queue->empty()) {
      Webcam::wait(); // wait until queue is empty
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
    const Webcam::WebcamFrame &frame = t_video_data->get_data_ref();
    LOG_INFO("Loading frame size %lu", frame.size());
    // NOT DECODE AS cv::Mat and decode as a QT format or just push the jpeg.
    m_webcam.decode_one(frame, m_queue); // converts and pushes to video queue.
  }

}

auto VideoPlayback::get_stream() -> VideoQueuePtr { return m_queue; }

// void VideoPlayback::spawn_video_playback_thread() {

//   std::thread video_playback_thread([this]() {

//       std::size_t trials = 0;

//     while (m_status == Started) {

//       if(trials > m_MID_PLAYBACK_THROTTLE){
//        LOG_TRACE("Throttling video playback to 50ms");
//          std::this_thread::sleep_for(std::chrono::milliseconds(50));
//       }

//       if(trials > m_MAX_PLAYBACK_THROTTLE){
//        LOG_TRACE("Throttling video playback to 100ms");
//          std::this_thread::sleep_for(std::chrono::milliseconds(100));
//      }

//       if (m_queue->empty()) {
//         trials++;
//         continue;
//       }

//       Webcam::WebcamMat mat;

//       bool valid = m_queue->pop_try(mat);

//       if (valid) {
//         LOG_INFO("showing incoming video frame.");
//         trials = 0;
//          //    Webcam::show(mat); // NOTE: CANNOT SHOW IF NOT THE MAIN THREAD>
//       } 

//       Webcam::wait();
//     }
//   });

//   video_playback_thread.detach();
// }
