#include "av_stream.hpp"

AVStream::AVStream()
    : m_input(m_input_queue, AudioDevice::Input),
      m_output(m_output_queue, AudioDevice::Output) { }


void AVStream::start(){
  m_input.open();
  AudioDevice::wait(25); // wait one second to buffer and fill queue
}

void AVStream::stream(DataCallback &t_callback){

  while(!m_input_queue->empty()){
     Data::DataVector encoded_audio = m_converter.encode(m_input_queue);
     t_callback(std::move(encoded_audio));
  }

}

void AVStream::stop(){
  m_input.close();
}
