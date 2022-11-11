#include "webcam.hpp"
#include "av_settings.hpp"
#include "logger.hpp"
#include <exception>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
Webcam::Webcam() {}

Webcam::~Webcam() {

  if (m_capture.isOpened()) {
    LOG_INFO("Camera was not closed. Releasing in destructor.")
    stop();
  }
}

void Webcam::start() {

  m_VIDEO_SETTINGS = VideoSettings::get_instance();
  m_camera         = m_VIDEO_SETTINGS->camera();

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

void Webcam::stop() {
  LOG_INFO("Releasing webcam resources.")
  m_capture.release();
  cv::destroyAllWindows();
}

auto Webcam::capture_many(std::size_t t_nb_frames) -> Webcam::WebcamFrames {


  if (!m_valid) {
    LOG_ERR("Could not capture. Webcam in an invalid state.")
  }

  std::size_t index = 0;
  WebcamFrames frames_captured;
  frames_captured.reserve(t_nb_frames);

  while (m_valid && (t_nb_frames > index)) {

    m_capture.read(m_frame);

    if (m_frame.empty()) {
      LOG_ERR("Could not capture frame.");
      break;
    }

    std::vector<uchar> buffer;

    Data::DataVector data = capture_frame();

    if (m_valid) {
      frames_captured.push_back(std::move(data));
    }

    index++;
    // cv::imshow("Camera", m_frame);
    Webcam::wait();

  }
  
  return frames_captured;
}

/* */


auto Webcam::capture_one() -> WebcamFrame {


    WebcamFrame buffer;

 if (!m_valid) {
    LOG_ERR("Could not capture. Webcam in an invalid state.")
      return buffer;
  }

  m_capture.read(m_frame);

    if (m_frame.empty()) {
      LOG_ERR("Could not capture frame.");
      return buffer;
    }

   Data::DataVector data = capture_frame();

   return data;
};


/* */

auto Webcam::valid() const -> bool { return m_valid; }

/* */

void Webcam::convert(WebcamFrames &t_frames, CVMatQueue &t_output) {
  for (auto &frame : t_frames) {
    try {
      cv::Mat mat_frame = cv::imdecode(frame, cv::IMREAD_COLOR);
      t_output.push(mat_frame);

    } catch (...) {
      LOG_ERR("Error to decode frames.")
      m_valid = false;
    }
  }
}

/* Statics */

void Webcam::show(cv::Mat &t_frame) { cv::imshow("Camera", t_frame); };

/* */

void Webcam::wait() { // STATIC
  const VideoSettings *video_settings = VideoSettings::get_instance();
  /* 1000ms / 25f = 40ms  when framerate() = 25fs */
  cv::waitKey(1000 / video_settings->framerate());
}

/* Private */

auto Webcam::capture_frame() -> Data::DataVector {

  std::vector<uchar> buffer;

  if (m_valid) {
    m_capture.read(m_frame);

    if (m_frame.empty()) {
      LOG_ERR("Could not capture frame.");
      return buffer;
    }

    try {
      bool result = cv::imencode(m_VIDEO_SETTINGS->converter_type(), m_frame, buffer);
      if (!result) {
        LOG_ERR("Error to encode frames during capture.")
        m_valid = false;
      }

    } catch (std::exception &t_msg) {
      LOG_ERR("OpenCV threw on convertion:\n %s. ", t_msg.what());
      m_valid = false;
    }
  }

  return buffer;
}
