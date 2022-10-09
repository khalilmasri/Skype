#include "webcam.hpp"
#include "logger.hpp"
#include <exception>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

VideoSettings *Webcam::m_VIDEO_SETTINGS = VideoSettings::get_instance();

Webcam::Webcam() : m_camera(0) {

  m_capture.open(m_camera);

  if (!m_capture.isOpened()) {
    LOG_ERR("Failed to open Webcam.")
    m_valid = false;
  }

    LOG_INFO("Webcam has opened.")
 
 m_capture.set(cv::CAP_PROP_FRAME_WIDTH, m_VIDEO_SETTINGS->width());
 m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, m_VIDEO_SETTINGS->height());
 m_frame = cv::Mat::zeros(m_VIDEO_SETTINGS->height(), m_VIDEO_SETTINGS->width(), CV_8UC3);

}

Webcam::~Webcam() {

  LOG_INFO("Releasing webcam resources.")
  m_capture.release();
 cv::destroyAllWindows();
}

auto Webcam::capture() -> Webcam::WebcamFrames {

  if (!m_valid) {
    LOG_ERR("Could not capture. Webcame in an invalid state.")
  }

  int nb_frames = m_VIDEO_SETTINGS->capture_size_frames();
  int index = 0;
  WebcamFrames frames_captured;
  frames_captured.reserve(nb_frames);

  while (m_valid && (nb_frames > index)) {
    m_capture.read(m_frame);

    if (m_frame.empty()) {
      LOG_ERR("Could not capture frame.");
      break;
    }
    cv::imshow("Camera", m_frame);
    std::vector<uchar> buffer;

    try {
          bool result = cv::imencode(".jpeg", m_frame, buffer);
      if (result) {
        frames_captured.push_back(std::move(buffer));
      } else {
        LOG_ERR("Error to encode frames during capture.")
        m_valid = false;
      }
    } catch (std::exception &t_msg) {
      LOG_ERR("OpenCV threw on convertion:\n %s. ", t_msg.what());
      m_valid = false;
    }

    index++;

    /* 1000ms / 25f = 40ms */
    cv::waitKey(1000 / m_VIDEO_SETTINGS->framerate()); 
  }

  return frames_captured;
}

auto Webcam::valid() const -> bool { return m_valid; }
