#include "av_stream.hpp"
#include "doctest.h"
#include "logger.hpp"
#include <chrono>
#include <thread>

AVStream::AVStream(Webcam &t_webcam, StreamType t_type)
    : m_input(m_input_queue, AudioDevice::Input),  m_webcam(t_webcam), m_stream_type(t_type) {}

void AVStream::set_stream_type(StreamType t_type) {
  LOG_INFO("Setting stream type to : '%s'", t_type == Audio ? "Audio" : "Video");
  m_stream_type = t_type;
}

void AVStream::start() {
  if (m_status == Stopped) {

    if (m_stream_type == Audio) {
      m_input.open(); // open audio mic input
      AudioDevice::wait(1); //  wait a frame to fill the audio queue.
      m_status = Started;
    }

    if (m_stream_type == Video) {
      m_status = Started;
    }

  } else {
    LOG_ERR("Could not START start AVStream because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}

/* */

void AVStream::stream(P2PPtr &t_p2p_conn) {

  if (m_stream_type == Audio) {
    LOG_TRACE("Audio stream start.")
    stream_audio(t_p2p_conn);

  } else if( m_stream_type == Video){
    LOG_TRACE("Video stream start.")
    stream_video(t_p2p_conn);

  }
}

/* */

void AVStream::stop() {

  m_status = Stopped;

  if (m_status == Started) {
    if (m_stream_type == Audio) {
      m_input.close();
    }

    if (m_stream_type == Video && m_webcam.status() == Webcam::Ready) {
      LOG_INFO("AVStream releasing webcam...");
      m_webcam.release();
    }

    LOG_INFO("Closing %s AVStream...",
             m_stream_type == Audio ? "Audio" : "Video");

  } else {

    LOG_ERR("Cannot STOP %s AVStream because its status is: %s",
            m_stream_type == Audio ? "Audio" : "Video",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

/* */

void AVStream::stream_audio(P2PPtr &t_p2p_conn) {
  std::thread stream_thread([this, &t_p2p_conn]() {
    while (m_status == Started && t_p2p_conn != nullptr) {
      send_audio(t_p2p_conn);
    }

    // empty out the queue before stopping...
    while (!m_input_queue->empty()) {
      send_audio(t_p2p_conn);
    }

    if(t_p2p_conn != nullptr){
    send_done(t_p2p_conn); // sends a Data::Done package to peer
    }

  });

  stream_thread.detach();
}

/* */

void AVStream::stream_video(P2PPtr &t_p2p_conn) {

  std::thread stream_thread([this, &t_p2p_conn]() {
    Request req = t_p2p_conn->make_request();

    while (m_status == Started && t_p2p_conn != nullptr) {
      Webcam::WebcamFrame frame = m_webcam.capture_one();

      req.set_data(new AVData(std::move(frame), Data::Video));
      t_p2p_conn->send_package(req);
      Webcam::wait(); // wait one frame
    }

    if(t_p2p_conn != nullptr){
    send_done(t_p2p_conn); // sends a Data::Done package to peer
    }
  });

  stream_thread.detach();
}

/* */

void AVStream::send_audio(P2PPtr &t_p2p_conn) {

  std::vector<uint8_t> encoded_audio = m_converter.encode(m_input_queue);

  // if p2p connection is nil just convert just to empty queue but return
  if(t_p2p_conn == nullptr){
    return;
  }

  Request audio_req = t_p2p_conn->make_request();

  audio_req.set_data(new AVData(std::move(encoded_audio), Data::Audio));
  t_p2p_conn->send_package(audio_req);
}

/* this will send a Data::Done package to the peer letting it done the call is done */

void AVStream::send_done(P2PPtr &t_p2p_conn) {

  Request done_req = t_p2p_conn->make_request();

  done_req.set_data(new AVData(Data::Done));
  t_p2p_conn->send_package(done_req);
}

AVStream::StreamType AVStream::stream_type () const { return m_stream_type; };
