#include "job_queue.hpp"
#include "job.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

void JobQueue::push(Job &t_job){

    std::shared_ptr<Job> job(std::make_shared<Job>(std::move(t_job)));
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    
    m_queue.push(job);
}

bool JobQueue::pop_try(Job &t_job){

    std::lock_guard<std::mutex> lock_guard(m_mutex);

    if (true == m_queue.empty()){
        return false;
    }

    t_job = std::move(*m_queue.front());
    m_queue.pop();

    return true;
}

bool JobQueue::empty() {
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_queue.empty();
}