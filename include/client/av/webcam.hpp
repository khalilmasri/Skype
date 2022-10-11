#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <opencv2/opencv.hpp>
#include "av_settings.hpp"
#include "data.hpp"

class Webcam
{
public:
  using WebcamFrames = std::vector<std::vector<uchar>>;

  Webcam();
  ~Webcam();
  [[nodiscard]] auto capture() -> WebcamFrames;
  [[nodiscard]] auto capture_frame() -> Data::DataVector;
  [[nodiscard]] auto valid() const -> bool;
  static void wait();

private:
  int                m_camera;
  cv::Mat            m_frame;
  cv::VideoCapture   m_capture;
  bool               m_valid = true;

  static VideoSettings *m_VIDEO_SETTINGS;
};

#endif // WEBCAM_HPP
