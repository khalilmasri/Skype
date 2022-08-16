#include "program.hpp"
#include "client.hpp"
#include "job.hpp"
#include "job_queue.hpp"
#include <thread>

Program::Program() : m_bus() {

    
    // m_skype.run();

    Client client;
    
    std::thread bus_loop(&JobBus::main_loop);
    std::string username = "khalil";
    std::string password = "1234";
    
    Job job = {Job::SETUSER, "khalil"};
    jobQ.push(job);
    
    job = {Job::SETPASS, "1234"};
    jobQ.push(job);

    job = {Job::LOGIN};
    jobQ.push(job);
    
    job = {Job::GETUSER};
    jobQ.push(job);

    sleep(3);

    JobBus::set_exit();
    bus_loop.detach();
}

Program::~Program() {};