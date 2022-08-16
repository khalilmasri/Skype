#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include "job.hpp"

#include <queue>

struct JobQueue {
    std::queue<Job> jobs;
    size_t size = 0;
    void add_job(Job &t_job);
    bool pop_job(Job &t_job);
};

static JobQueue jobQ;

#endif // JOB_QUEUE_H