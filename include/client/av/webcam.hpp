#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <opencv2/opencv.hpp>
#include <queue>
#include "av_settings.hpp"
#include "data.hpp"


class Webcam
{
public:
  using WebcamFrames = std::vector<std::vector<uchar>>;
  using CVMatQueue = std::queue<cv::Mat>;

  Webcam();
  ~Webcam();
  [[nodiscard]] auto capture() -> WebcamFrames;
  [[nodiscard]] auto valid() const -> bool;
  void               convert(WebcamFrames &t_frames, CVMatQueue &t_output);

  static void show(cv::Mat &t_frame);
  void start();

  static void wait();

private:
  int                m_camera;
  cv::Mat            m_frame;
  cv::VideoCapture   m_capture;
  bool               m_valid = true;

  auto capture_frame() -> Data::DataVector;

  VideoSettings *m_VIDEO_SETTINGS;
};

#endif // WEBCAM_HPP
