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

//  std::string gs_pipeline = "v4l2src device=/dev/video0 ! videoconvert! videoscale ! video/x-raw, width=2592, height=600 ! appsink";

  m_capture.open(m_camera);

  if (!m_capture.isOpened()) {
    LOG_ERR("Failed to open Webcam.")
    m_valid = false;
  }

  if (m_capture.isOpened()) {
    std::cout << "it is opened!" << std::endl;
  }

 m_capture.set(cv::CAP_PROP_FRAME_WIDTH, m_VIDEO_SETTINGS->width());
 m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, m_VIDEO_SETTINGS->height());
 m_frames = cv::Mat::zeros(m_VIDEO_SETTINGS->height(), m_VIDEO_SETTINGS->width(), CV_8UC3);

}

Webcam::~Webcam() {
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
    m_capture.read(m_frames);

    if (m_frames.empty()) {
      LOG_ERR("Could not capture frame.");
      break;
    }

    cv::imshow("Camera", m_frames);
    std::vector<uchar> buffer;

    try {
          bool result = cv::imencode(".jpeg", m_frames, buffer);
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
  }

  return frames_captured;
}

void Webcam::wait() { cv::waitKey(25); }
auto Webcam::valid() const -> bool { return m_valid; }
