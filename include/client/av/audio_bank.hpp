#ifndef AUDIO_BANK_H
#define AUDIO_BANK_H
#include "data.hpp"
#include "av_settings.hpp"
#include "logger.hpp"
#include <queue>

class AudioBank{
  public:
  AudioBank(){
    auto *audio_settings = AudioSettings::get_instance();
    auto *video_settings = VideoSettings::get_instance();
     // 7056 bytes
     m_buffer_size = (audio_settings->samplerate() / video_settings->framerate()) * audio_settings->bit_multiplier(); 
    // m_item.reserve(m_buffer_size);
  }

  void load(Data::DataVector &&t_data){

    if(t_data.size() > m_buffer_size){
      LOG_ERR("Attempted to agreggate data that is larger than buffer size. data_size: %lu , buffer_size: %lu", t_data.size(), m_buffer_size);
      return;
    }

    if(m_item.empty()){
      m_item = std::move(t_data);
      return;
    } 

    std::size_t data_index = 0;
    data_index = copy_data(t_data, data_index);
  
    if(data_index < t_data.size()){
      m_store.push(std::move(m_item));
      m_item = {};

      data_index = copy_data(t_data, data_index);
    }

    if(m_item.size() == m_buffer_size){
      m_store.push(std::move(m_item));
      m_item = {};
    }
  }

  Data::DataVector grab(){

    if(m_store.empty()){
      LOG_ERR("m_store is empty.");
      return {};
    }

    Data::DataVector data = m_store.front();
    m_store.pop();

    return data;
  }


  std::size_t get_store_size(){
    return m_store.size();
  }

  // load() ---
  // grab() ---

private:
  std::queue<Data::DataVector> m_store; // push here when m_tiem is completed.
  Data::DataVector m_item; // keep here until is has the full size.
  std::size_t m_buffer_size;

  std::size_t copy_data(Data::DataVector &t_data, std::size_t t_index){

    std::size_t index = t_index;

   while(m_item.size() < m_buffer_size){

      if(index >= t_data.size()){
        break;
      }

      m_item.push_back(t_data.at(index));
      index++;
    }

   return index;
  }

};

#endif // !AUDIO_BANK_H
