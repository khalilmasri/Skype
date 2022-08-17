#include "program.hpp"
#include "client.hpp"
#include "job.hpp"
#include "job_queue.hpp"
#include "fail_if.hpp"

#include <thread>


Program::Program() {

    // m_skype.run();
    Client client;
    Job job;
    
    std::thread bus_loop(&JobBus::main_loop);
    
    FAIL_IF_MSG(false == JobBus::handle({Job::SETUSER, "khalil"}), "Failed to set user");
    
    FAIL_IF_MSG(false == JobBus::handle({Job::SETPASS, "1234"}), "Failed to set password");

    FAIL_IF_MSG(false == JobBus::handle({Job::LOGIN}), "Failed to login");
    
    job = {Job::GETUSER};
    FAIL_IF_MSG(false == JobBus::handle(job), "Failed to get user");
    std::cout << "username => " << job.m_string << std::endl;

    sleep(8);

fail:
    bus_loop.detach();
    return;
}

Program::~Program() {};
