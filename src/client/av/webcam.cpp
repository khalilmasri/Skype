#include "webcam.hpp"
#include "av_settings.hpp"
#include "logger.hpp"
#include <exception>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include <exception>

Webcam::Webcam() {}

Webcam::~Webcam() {

  if (m_capture.isOpened()) {
    LOG_INFO("Camera was not closed. Releasing in destructor.")
    release();
  }
}

void Webcam::init() {

  if(m_status == Ready){
    LOG_ERR("Webcam is has already been initialized. Ignoring Webcam::init call.")
  }

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
  m_status = Ready;

}

void Webcam::release() {

  if(m_status == Uninitialized){
    LOG_ERR("Cannot release Webcam because status = 'Uninitialized'.");
  }

  LOG_INFO("Releasing webcam resources.")
  m_capture.release();
  cv::destroyAllWindows();
  m_status = Uninitialized;
}

auto Webcam::capture_many(std::size_t t_nb_frames) -> Webcam::WebcamFrames {

  if(m_status == Uninitialized){
    log_error("Could not capture many. Webcam is Uninitialized. Returning empty WebcamFrames.");
    count_logs();
    return {};
  }

  if (!m_valid) {
    log_error("Could not capture many. Webcam in an invalid state. Returning empty WebcamFrames.");
    count_logs();
    return {};
  }

  std::size_t index = 0;
  WebcamFrames frames_captured;
  frames_captured.reserve(t_nb_frames);

  while (m_valid && (t_nb_frames > index)) {

    m_capture.read(m_frame);

    if (m_frame.empty()) {
      log_error("Could not capture frame. m_frame returned empty from m_capture.read.");
      count_logs();
      break;
    }

    std::vector<uchar> buffer;

    Data::DataVector data = encode_frame();

    if (m_valid) {
      frames_captured.push_back(std::move(data));
    } else {

      LOG_DEBUG("Setting m_valid to true after encode attempt.")
      m_valid =  true;
    }

    index++;
    Webcam::wait();

  }
  
  return frames_captured;
}

/* */


auto Webcam::capture_one() -> WebcamFrame {

    WebcamFrame buffer;

 if (!m_valid) {
      log_error("Could not capture. Webcam in an invalid state.");
      count_logs();
      return buffer;
  }

  if(m_status == Uninitialized){
      log_error("Webcam has not been initialized. Cannot capture.");
      count_logs();
      return buffer;
  }

   m_capture.read(m_frame);

    if (m_frame.empty()) {
      log_error("Could not capture frame. m_frame returned empty from m_capture.read.");
      count_logs();
      return buffer;
    }

   Data::DataVector data = encode_frame();

   if(!m_valid){
     LOG_DEBUG("Setting valid to true after encode attempt.");
     m_valid = true;
   }

   return data;
};


/* */

auto Webcam::valid() const -> bool { return m_valid; }
auto Webcam::status() const -> Status { return m_status; }

/* */

void Webcam::make_valid(const char *t_from){
  LOG_INFO("'%s' is setting Webcam state from 'Invalid' to 'Valid'.", t_from);
  m_valid = true;
}

/* */

void Webcam::decode_many(WebcamFrames &t_frames, std::shared_ptr<CVMatQueue> t_output) {

  for (auto &frame : t_frames) {
      decode_one(frame, t_output);
  }
}

/* */

void Webcam::decode_one(const WebcamFrame &t_frame, std::shared_ptr<CVMatQueue> t_output) {

  if(t_frame.empty()){
    log_error("Provided jpeg frame buffer was empty.");
    count_logs();
    return;
  }

    try {
      cv::Mat mat_frame = cv::imdecode(t_frame, cv::IMREAD_COLOR);

      if (mat_frame.data == nullptr){
         log_error("Error to decoding frame. Ignoring video package.");
         count_logs();
       
      } else {
        try {
        t_output->push(mat_frame);
        } catch(...){
          std::cout << "CAUGHT IN POP PUSH!!!!";
        }
      }

    } catch (std::exception &err) {
       std::string msg = err.what();
       log_error("cv::imdecode threw on decoding: '" + msg + "'");
       count_logs();
    }
}


/* */

void Webcam::count_logs(){

  if(m_log_count > m_MAX_LOG_COUNT){
    m_suppress_log = true;
  }

  m_log_count++;
}


/* */

void Webcam::log_error(const char *t_msg){
  if(!m_suppress_log){
    LOG_ERR("%s", t_msg);
  }
}

/* */

void Webcam::log_error(std::string &&t_msg){
  if(!m_suppress_log){
    LOG_ERR("%s", t_msg.c_str());
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

auto Webcam::encode_frame() -> Data::DataVector {

  std::vector<uchar> buffer;

  if (m_valid) {
    m_capture.read(m_frame);

    if (m_frame.empty()) {
        log_error("Cannot encode an empty frame. Returning an empty jpeg buffer...");
        count_logs();
        m_valid = false;
        return buffer;
    }

    try {
      bool result = cv::imencode(m_VIDEO_SETTINGS->converter_type(), m_frame, buffer);

      if (!result) {
        log_error("Error while encoding frames during capture. Returning empty jpeg buffer.");
        count_logs();
        m_valid = false;
      }

    } catch (std::exception &t_msg) {
      std::string msg = t_msg.what();
      log_error("cv::imencode threw on encoding: '" + msg + "'");
      count_logs();
      m_valid = false;
    }
  }

  return buffer;
}
