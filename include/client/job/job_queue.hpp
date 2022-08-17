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
    JobQueue();

    static void push(Job &t_job);
    static bool pop_try(Job &t_job);
    static void push_res(Job &t_job);
    static bool pop_res(Job &t_job);
    static bool empty();

private:

    inline static std::mutex m_job_mutex;
    inline static std::mutex m_res_mutex;
    inline static std::queue<std::shared_ptr<Job>> m_queue;
    inline static std::queue<std::shared_ptr<Job>> m_res;
    inline static std::condition_variable m_datacond;  
};

static JobQueue jobQ;

#endif // JOB_QUEUE_H