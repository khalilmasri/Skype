#include "av_stream.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <chrono>
#include <thread>

AVStream::AVStream() : m_input(m_input_queue, AudioDevice::Input) {}

void AVStream::start() {
  if (m_status == Stopped) {
    m_input.open();        // audio
    m_webcam.start();
    AudioDevice::wait(50); // frames
    m_status = Started;

  } else {
    LOG_ERR("Could not START start AVStream because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}

void AVStream::stream(DataCallback &t_callback) {

  while (m_status == Started) {

    // take a frame and wait 1 frame worth of time
    Webcam::WebcamFrames encoded_video = m_webcam.capture();

    // convert first audio buffer from the audio queue.
    std::vector<uint8_t> encoded_audio = m_converter.encode(m_input_queue);

    // check if conversion and frame capture were successful
    validate();

    t_callback(std::move(encoded_video), std::move(encoded_audio));
  }
}

void AVStream::stop() {
  if (m_status == Started) {
    m_input.close();
    m_webcam.stop();
    m_status = Stopped;
  } else {
    LOG_ERR("Cannot STOP AVStream because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void AVStream::validate() {
  if (!(m_webcam.valid() && m_converter.valid())) {
    m_status = Invalid;
  }
};

using namespace std::chrono_literals;

//TEST_CASE("AVStreaming") {
//
//  auto stream = AVStream();
//  auto stop = [&stream]() {
//    std::this_thread::sleep_for(2s); // do it for 10 secs then quit.
//    stream.stop();
//  };
//
//
//  AVStream::DataCallback cb = [](Webcam::WebcamFrames &&t_video,
//                                 Data::DataVector &&t_audio) {
//    std::cout << "frames: " << t_video.size() << std::endl;
//    std::cout << "audio bites: " << t_audio.size() << std::endl;
//  };
//
//  SUBCASE("Testing case") {
//
//    stream.start();
//    std::thread t(stop);
//
//    stream.stream(cb);
//    
//    t.join();
//    // CHECK(2 == 1);
//  }
//}
