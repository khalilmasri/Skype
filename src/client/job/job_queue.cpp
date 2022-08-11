#include "job_queue.hpp"
#include "job.hpp"

void JobQueue::add_job(Job& t_job) {
    jobQ.jobs.push(t_job);
}

bool JobQueue::pop_job(Job &t_job) {
    t_job = jobQ.jobs.front();
    jobQ.jobs.pop();
    return true;
}