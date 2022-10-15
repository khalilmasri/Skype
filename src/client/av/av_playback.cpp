#include "av_playback.hpp"
#include "logger.hpp"

AVPlayback::AVPlayback() : m_audio_output(m_audio_queue, AudioDevice::Output) {}

void AVPlayback::start(P2PPtr &t_p2pconn) {
  if (m_status == Stopped) {
    m_audio_output.open(); // Audio
    m_status = Started;

    while (m_status == Started) {

      // show frame than pop from the queue
      cv::Mat frame = m_video_queue.front();
      Webcam::show(frame);
      m_video_queue.pop();

      // read_package from network between frames
      read_package_for(t_p2pconn);

    };

  } else {
    LOG_ERR("Could not START audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Started'.")
  }
}
void AVPlayback::stop() {

  if (m_status == Started) {

    // wait for the queue to be empty before stopping.
    while (!m_audio_queue->empty()) {
      AudioDevice::wait(1); // wait 1 frame
    }

    m_audio_output.close();
    m_status = Stopped;
  } else {
    LOG_ERR("Could not STOP audio player because its status is: %s",
            m_status == Invalid ? "'Invalid'." : "'Stopped'.")
  }
}

void AVPlayback::buffer(P2PPtr &t_p2p_conn, std::size_t nb_packages) {
  for (std::size_t count = 0; count < nb_packages; count++) {
    read_package(t_p2p_conn);
  }
};

void AVPlayback::read_package_for(P2PPtr &t_p2pconn, std::size_t nb_frames) {

  auto *video_settings = VideoSettings::get_instance();
  auto now            = std::chrono::steady_clock::now();

  /* calculate milliseconds based on the framerate. */
  auto deadline       = std::chrono::milliseconds ( (1000 / video_settings->framerate()) * nb_frames);

  /* read packages until deadline */
  /* if there is nothing to receive read_package may take longer than deadline */
  while (since(now).count() < deadline.count()) {
    read_package(t_p2pconn);
  };

}

void AVPlayback::read_package(P2PPtr &t_p2pconn) {

  auto *video_settings          = VideoSettings::get_instance();
  int capture_size              = video_settings->capture_size_frames();
  Request req                   = t_p2pconn->make_request();
  int read_size                 = 0;

  Webcam::WebcamFrames frames;
  frames.reserve(capture_size);

  /* we should read exactly the number of frames captured by the other client */
  while (read_size < capture_size) {

    /* receive package from network and validate if it is the correct data type */
    t_p2pconn->receive_package(req);
    const Data *data = req.data();

   /* if empty try again  */
    if (req.data_type() == Data::Empty) {
      continue;
    }

    /* There is an error. Wrong data type. */
    if (!valid_data_type(data, Data::Video)) {
      break;
    }

    frames.push_back(data->get_data());
    read_size++;
  };

  LOG_DEBUG("read %d video frames from socket.", read_size);

  /* convert and push  franes to video queue buffer */
  if (m_status != Invalid) {
    m_webcam.convert(frames, m_video_queue);
  }

  
  /* If package is empty,read again until we get data */
  if (req.data_type() == Data::Empty) {
    while(req.data_type() == Data::Empty) {
      t_p2pconn->receive_package(req);
    }
  }

  const Data *audio_data = req.data();
  load_audio(audio_data);
}

void AVPlayback::load_audio(const Data *t_audio_data) {

  if (valid_data_type(t_audio_data, Data::Audio)) {

    Data::DataVector audio = t_audio_data->get_data();
    bool result = m_audio_converter.decode(m_audio_queue, audio);

    if (!result) {
      LOG_ERR("Error converting audio to output.");
    }
  }
}

auto AVPlayback::valid_data_type(const Data *t_data, Data::type t_type)
    -> bool {

  if (t_data->get_type() != t_type) {
    std::string data_type = Data::type_to_string(t_data->get_type());
    std::string desired = Data::type_to_string(t_type);
    LOG_ERR("Attempted to load AVPlayback with the wrong that type. Was: '%s' "
            "Should be:  '%s'.",
            data_type.c_str(), desired.c_str());
    m_status = Invalid;
    return false;
  }

  return true;
}

auto AVPlayback::valid_read_size(int t_read_size, int t_correct_size) -> bool {

  if (t_read_size > t_correct_size) {
    LOG_ERR("Video packages received exceeds "
            "VideoSettings::capture_size_frames of %d",
            t_correct_size);
    m_status = Invalid;
    return false;
  }

  return true;
}
