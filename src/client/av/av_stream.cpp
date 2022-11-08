#include "av_stream.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <chrono>
#include <thread>

AVStream::AVStream(StreamType t_type)
    : m_input(m_input_queue, AudioDevice::Input), m_stream_type(t_type) {}

void AVStream::set_stream_type(StreamType t_type) { m_stream_type = t_type; }

void AVStream::start() {
  if (m_status == Stopped) {

    if (m_stream_type == Audio) {
      m_input.open();
      AudioDevice::wait(1); // frames
      m_status = Started;
    }

    if (m_stream_type == Video) {
      m_webcam.start();
      m_status = Started;
    }

  } else {
    LOG_ERR("Could not START start AVStream because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}

void AVStream::stream(P2PPtr &t_p2p_conn) {

  if (m_stream_type != Audio) {
    LOG_ERR("Attempted to stream audio from a non-audio stream.");
    return;
  }

  std::thread stream_thread([this, &t_p2p_conn]() {

    while (m_status == Started) {
      send_audio(t_p2p_conn);
    }

    // empty out the queue before stopping...
    while(!m_input_queue->empty()){ 
      send_audio(t_p2p_conn);
    }

    send_done(t_p2p_conn); // sends a Data::Done package to peer

  });

  stream_thread.detach();
}


void AVStream::stop() {

  if (m_status == Started) {
    if (m_stream_type == Audio) {
      m_input.close();
    }

    if (m_stream_type == Video) {
      m_webcam.stop();
    }

    m_status = Stopped;

  } else {
    LOG_ERR("Cannot STOP AVStream because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void AVStream::send_audio(P2PPtr &t_p2p_conn){

     std::vector<uint8_t> encoded_audio = m_converter.encode(m_input_queue);
     Request audio_req                 = t_p2p_conn->make_request();

      audio_req.set_data(new AVData(std::move(encoded_audio), Data::Audio));
      t_p2p_conn->send_package(audio_req);
}


/* this will send a Data::Done package to the peer letting it done the call is done */
void AVStream::send_done(P2PPtr &t_p2p_conn){

      Request done_req = t_p2p_conn->make_request();

      done_req.set_data(new AVData(Data::Done));
      t_p2p_conn->send_package(done_req);
}



// TEST_CASE("AVStreaming") {
//
//   auto stream = AVStream();
//   auto stop = [&stream]() {
//     std::this_thread::sleep_for(2s); // do it for 10 secs then quit.
//     stream.stop();
//   };
//
//
//   AVStream::DataCallback cb = [](Webcam::WebcamFrames &&t_video,
//                                  Data::DataVector &&t_audio) {
//     std::cout << "frames: " << t_video.size() << std::endl;
//     std::cout << "audio bites: " << t_audio.size() << std::endl;
//   };
//
//   SUBCASE("Testing case") {
//
//     stream.start();
//     std::thread t(stop);
//
//     stream.stream(cb);
//
//     t.join();
//     // CHECK(2 == 1);
//   }
// }
