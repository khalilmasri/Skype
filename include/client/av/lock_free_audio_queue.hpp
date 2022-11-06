#ifndef LOCK_FREE_AUDIO_QUEUE_H
#define LOCK_FREE_AUDIO_QUEUE_H

#include <atomic>
#include <memory>
#include <vector>

/*
 *  This lock free queue assumes that there is a single consumer.
 *  Multiple consumer would require reference counting that would make_shared
 *  this implementation more complex.
 * */

struct AudioPackage {
  std::vector<uint8_t> m_data;
  int m_len = 0;
  int m_index = 0;

  AudioPackage();
  AudioPackage(std::vector<uint8_t> &&t_data);
  AudioPackage(uint8_t *t_stream, int t_len);

  void push_back(uint8_t t_value);

};

class LockFreeAudioQueue {

public:
  LockFreeAudioQueue();

  /* delete copy contructor */
  LockFreeAudioQueue(const LockFreeAudioQueue &t_other) = delete;
  auto operator=(const LockFreeAudioQueue &t_other) -> LockFreeAudioQueue & = delete;

  ~LockFreeAudioQueue();

  void push(AudioPackage &&t_audio_package);
  auto pop() -> std::shared_ptr<AudioPackage>;
  auto empty() -> bool;

private:
  struct Node {
    std::shared_ptr<AudioPackage> m_audio_package;
    Node *m_next = nullptr;
    Node() = default;
  };

  std::atomic<Node *> m_head = nullptr;

  std::atomic<Node *> m_tail = nullptr;
  auto pop_head() -> Node *;
};

#endif // !LOCK_FREE_AUDIO_QUEUE_H
