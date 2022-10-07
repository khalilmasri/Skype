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

  AudioPackage(int t_len);
  AudioPackage(uint8_t *t_stream, int t_len);

  void push_back(uint8_t t_value);

};

class LockFreeAudioQueue {

public:
  LockFreeAudioQueue();

  /* delete copy contructor */
  LockFreeAudioQueue(const LockFreeAudioQueue &t_other) = delete;
  LockFreeAudioQueue &operator=(const LockFreeAudioQueue &t_other) = delete;

  ~LockFreeAudioQueue();

  void push(AudioPackage &&t_audio_package);
  std::shared_ptr<AudioPackage> pop();
  bool empty();

private:
  struct Node {
    std::shared_ptr<AudioPackage> m_audio_package;
    Node *m_next = nullptr;
    Node() : m_next(nullptr){};
  };

  std::atomic<Node *> m_head = nullptr;

  std::atomic<Node *> m_tail = nullptr;
  Node *pop_head();
};

#endif // !LOCK_FREE_AUDIO_QUEUE_H
