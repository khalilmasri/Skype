#include "av_stream.hpp"

AVStream::AVStream()
    : m_input(m_input_queue, AudioDevice::Input) { }


void AVStream::start(){
  m_input.open(); // audio
  m_status = Started;
}

void AVStream::stream(DataCallback &t_callback){

  while(m_status == Started){

     // take a frame and wait 1 frame worth of time
      Webcam::WebcamFrames encoded_video = m_webcam.capture(); 
     
     // convert first audio buffer from the audio queue.
     Data::DataVector encoded_audio = m_converter.encode(m_input_queue);

     // check if conversion and frame capture were successful
     validate(); 

     t_callback(std::move(encoded_video), std::move(encoded_audio));
  }

}

void AVStream::stop(){
  m_input.close();
  m_status = Stopped;
}


void AVStream::validate(){
  if (!(m_webcam.valid() && m_converter.valid())){
    m_status = Invalid;
  }
};
