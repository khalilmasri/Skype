#include "job_queue.hpp"
#include "job.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

JobQueue::JobQueue(){};

void JobQueue::push(Job &t_job){

    std::shared_ptr<Job> job(std::make_shared<Job>(std::move(t_job)));
    std::lock_guard<std::mutex> lock_guard(m_job_mutex);
    
    m_queue.push(job);
}

bool JobQueue::pop_try(Job &t_job){

    std::lock_guard<std::mutex> lock_guard(m_job_mutex);

    if (true == m_queue.empty()){
        return false;
    }

    t_job = std::move(*m_queue.front());
    m_queue.pop();

    return true;
}

void JobQueue::push_res(Job &t_job){

    std::shared_ptr<Job> job(std::make_shared<Job>(std::move(t_job)));
    std::unique_lock<std::mutex> lock_guard(m_res_mutex);

    m_res.push(job);
    m_datacond.notify_one();
}

bool JobQueue::pop_res(Job &t_job){
    
    std::unique_lock<std::mutex> lock_guard(m_res_mutex);
    m_datacond.wait(lock_guard);

    if (true == m_res.empty()) {
        return false;
    }

    t_job = std::move(*m_res.front());
    m_res.pop();

    return true;
}

bool JobQueue::empty() {
    std::lock_guard<std::mutex> lock_guard(m_job_mutex);
    return m_queue.empty();
}