#include "job_queue.hpp"
#include "job.hpp"
#include <iostream>


void JobQueue::add_job(Job &t_job) {
    
    std::cout << "job queue " << t_job.command << std::endl;
    jobs.push(t_job);
    size++;
}

bool JobQueue::pop_job(Job &t_job) {
    t_job = jobs.front();
    jobs.pop();
    size--;
    return true;
}