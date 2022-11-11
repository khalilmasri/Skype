#ifndef WEBCAM_HPP
#define WEBCAM_HPP

#include <opencv2/opencv.hpp>
#include "thread_safe_queue.hpp"
#include "av_settings.hpp"
#include "data.hpp"


class Webcam
{
public:
  using WebcamFrames = std::vector<std::vector<uchar>>;
  using WebcamFrame  = std::vector<uchar>;
  using WebcamMat    = cv::Mat;
  using CVMatQueue   = ThreadSafeQueue<cv::Mat>;

  enum Status  {Uninitialized, Ready};

  Webcam();
  ~Webcam();
  [[nodiscard]] auto capture_many(std::size_t t_nb_frames) -> WebcamFrames;
  [[nodiscard]] auto capture_one() -> WebcamFrame;
  [[nodiscard]] auto valid() const -> bool;
  [[nodiscard]] auto status() const -> Status;
  void               decode_one(WebcamFrame &t_frame, CVMatQueue &t_output);
  void               decode_many(WebcamFrames &t_frames, CVMatQueue &t_output);
  void               release();

  static void show(cv::Mat &t_frame);

  void init();

  static void wait();

private:
  int                m_camera;
  cv::Mat            m_frame;
  cv::VideoCapture   m_capture;
  bool               m_valid = true;
  Status             m_status = Uninitialized;

  auto capture_frame() -> Data::DataVector;

  VideoSettings *m_VIDEO_SETTINGS;
};

#endif // WEBCAM_HPP
