#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

//#include "job.hpp"

#include <memory>
#include <queue>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
template <typename T> class ThreadSafeQueue {

public:
  void push(T &t_job) {
    std::shared_ptr<T> job(std::make_shared<T>(std::move(t_job)));
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    m_queue.push(job);
  }

  auto pop_try(T &t_job) -> bool {
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    if (!m_queue.empty()) {
      
      t_job = std::move(*m_queue.front());
      m_queue.pop();
      return true;
    }

    return false;
  }
  auto empty() -> bool {
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_queue.empty();
  }

private:
  std::mutex m_mutex;
  std::queue<std::shared_ptr<T>> m_queue;
};

#endif // JOB_QUEUE_H
