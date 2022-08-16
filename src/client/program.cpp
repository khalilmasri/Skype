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
    void* noth = nullptr;
    Job job1 = {Job::SETUSER, username, noth};
    jobQ.push(job1);
    sleep(5);
    std::cout << "Add user" << job1.return_value << std::endl;
    Job job2 = {Job::SETPASS, password, noth};

    while (true){}
    bus_loop.detach();
}

Program::~Program() {};