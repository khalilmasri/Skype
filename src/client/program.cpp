#include "program.hpp"
#include "client.hpp"
#include "job.hpp"
#include "job_queue.hpp"
#include "fail_if.hpp"

#include <thread>

bool JobHandle(Job & t_job){
    jobQ.push(t_job);
    jobQ.pop_res(t_job);

    return t_job.bool_value;
}

Program::Program() : m_bus() {

    
    // m_skype.run();

    Client client;
    
    std::thread bus_loop(&JobBus::main_loop);
    std::string username = "khalil";
    std::string password = "1234";
    
    Job job = {Job::SETUSER, "khalil"};
    FAIL_IF_MSG(false == JobHandle(job), "Failed to set user");
    std::cout << "Set user = > " << job.bool_value << std::endl;
    
    job = {Job::SETPASS, "1234"};
    FAIL_IF_MSG(false == JobHandle(job), "Failed to set password");
    std::cout << "Set pass = > " << job.bool_value << std::endl;

    job = {Job::LOGIN};
    FAIL_IF_MSG(false == JobHandle(job), "Failed to login");
    std::cout << "Login = > " << job.bool_value << std::endl;
    
    job = {Job::GETUSER};
    FAIL_IF_MSG(false == JobHandle(job), "Failed to get username");
    std::cout << "Get user = > " << job.s_value << std::endl;

    bus_loop.detach();
    return;
fail:
    bus_loop.detach();
    return;
}

Program::~Program() {};