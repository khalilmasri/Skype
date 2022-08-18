#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include "job.hpp"

#include <queue>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

class JobQueue {

public:
    void push(Job &t_job);
    bool pop_try(Job &t_job);
    bool empty();

private:

    std::mutex m_mutex;
    std::queue<std::shared_ptr<Job>> m_queue;
    std::condition_variable m_datacond;  
};
 

#endif // JOB_QUEUE_H