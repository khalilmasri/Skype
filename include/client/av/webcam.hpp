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

  /* Public */
  Webcam();
  ~Webcam();
  [[nodiscard]] auto capture_many(std::size_t t_nb_frames) -> WebcamFrames;
  [[nodiscard]] auto capture_one() -> WebcamFrame;
  [[nodiscard]] auto valid() const -> bool;
  [[nodiscard]] auto status() const -> Status;
  void               decode_one(const WebcamFrame &t_frame, std::shared_ptr<CVMatQueue> t_output);
  void               decode_many(WebcamFrames &t_frames, std::shared_ptr<CVMatQueue> t_output);
  void               release();
  void               init();
  void               make_valid(const char *t_from);

  /* Statics */
  static void        show(cv::Mat &t_frame);
  static void        wait();

private:
  int                m_camera;
  cv::Mat            m_frame;
  cv::VideoCapture   m_capture;
  bool               m_valid        = true;
  Status             m_status       = Uninitialized;
  bool               m_suppress_log = false;
  std::size_t        m_log_count    = 0;

  auto encode_frame() -> Data::DataVector;
  void log_error(const char *t_msg);
  void log_error(std::string &&t_msg);
  void count_logs();

  VideoSettings *m_VIDEO_SETTINGS;

  inline static const std::size_t m_MAX_LOG_COUNT = 20;
};

#endif // WEBCAM_HPP
