#include "lock_free_audio_queue.hpp"
#include <iostream>
#include "av_settings.hpp"

AudioPackage::AudioPackage(){ 
  auto *config = AudioSettings::get_instance();
  m_len =  config->buffer_size();
  m_data.reserve(m_len); 
}

AudioPackage::AudioPackage(std::vector<uint8_t> &&t_data): m_data(std::move(t_data)), m_len(m_data.size()){
}

AudioPackage::AudioPackage(uint8_t *t_stream, int t_len) : m_len(t_len) {

  m_data.reserve(m_len);

  int index;

  for (index = 0; index < m_len; index++) {
    m_data.push_back(t_stream[index]);
  }
  m_index = index;
}

/* */

void AudioPackage::push_back(uint8_t t_value) {
  m_data.push_back(t_value);
  m_index++;
}


/* Queue */
LockFreeAudioQueue::LockFreeAudioQueue() : m_head(new Node) {
  m_tail = m_head.load();
};

LockFreeAudioQueue::~LockFreeAudioQueue() { // free queue
  while (const Node *old_head = m_head.load()) {
    m_head.store(old_head->m_next);
    delete old_head;
  }
};

/* */

void LockFreeAudioQueue::push(AudioPackage &&t_audio_package) {
  auto new_data = std::make_shared<AudioPackage>(t_audio_package);
  Node *new_node = new Node;

  Node *const old_tail = m_tail.load();

  old_tail->m_audio_package.swap(new_data); // new data tail
  old_tail->m_next = new_node;
  m_tail.store(new_node); // update new tail
};

/* */

auto LockFreeAudioQueue::pop() -> std::shared_ptr<AudioPackage> {

  Node *old_head = pop_head();

  if (old_head == nullptr) {
    return {};  // if empty return empty shared ptr
  }

  std::shared_ptr<AudioPackage> result(old_head->m_audio_package);
  delete old_head;
  return result;
}

/* */

auto LockFreeAudioQueue::pop_head() -> LockFreeAudioQueue::Node * {

  Node *const old_head = m_head.load();

  if (old_head == m_tail.load()) { // is empty
    return nullptr;
  }

  m_head.store(old_head->m_next); // pop from queue
  return old_head;
}

/* */

auto LockFreeAudioQueue::empty() -> bool { return m_head.load() == m_tail.load(); }
