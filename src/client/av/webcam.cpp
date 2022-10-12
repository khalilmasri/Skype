#include "webcam.hpp"
#include "logger.hpp"
#include <exception>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>

// VideoSettings *Webcam::m_VIDEO_SETTINGS = VideoSettings::get_instance();

Webcam::Webcam() : m_camera(0) {

  m_capture.open(m_camera);

  if (!m_capture.isOpened()) {
    LOG_ERR("Failed to open Webcam.")
    m_valid = false;
  }

  LOG_INFO("Webcam has opened.")

  const VideoSettings *video_settings = VideoSettings::get_instance();

  m_capture.set(cv::CAP_PROP_FRAME_WIDTH, video_settings->width());
  m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, video_settings->height());
  m_frame = cv::Mat::zeros(video_settings->height(), video_settings->width(),
                           CV_8UC3);
}

Webcam::~Webcam() {

  LOG_INFO("Releasing webcam resources.")
  m_capture.release();
  cv::destroyAllWindows();
}

auto Webcam::capture() -> Webcam::WebcamFrames {

  const VideoSettings *video_settings = VideoSettings::get_instance();

  if (!m_valid) {
    LOG_ERR("Could not capture. Webcam in an invalid state.")
  }

  int nb_frames = video_settings->capture_size_frames();
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

    Data::DataVector data = capture_frame();

    if (m_valid) {
      frames_captured.push_back(std::move(data));
    }

    index++;

    Webcam::wait();
  }

  return frames_captured;
}

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

void Webcam::wait() {

  const VideoSettings *video_settings = VideoSettings::get_instance();
  /* 1000ms / 25f = 40ms  when framerate() = 25fs */
  cv::waitKey(1000 / video_settings->framerate());
}

/* Private */

auto Webcam::capture_frame() -> Data::DataVector {

  const VideoSettings *video_settings = VideoSettings::get_instance();

  std::vector<uchar> buffer;

  if (m_valid) {
    m_capture.read(m_frame);

    if (m_frame.empty()) {
      LOG_ERR("Could not capture frame.");
      return buffer;
    }

    try {
      bool result =
          cv::imencode(video_settings->converter_type(), m_frame, buffer);
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
