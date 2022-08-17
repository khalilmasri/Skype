#include "program.hpp"
#include "client.hpp"
#include "job.hpp"
#include "job_queue.hpp"
#include "fail_if.hpp"

#include <thread>


Program::Program(){

    std::thread bus_loop(&JobBus::main_loop);
    Client client;
    
    // std::thread skype_loop([this]{m_skype.run();});
    m_skype.run();

fail:
    bus_loop.detach();
    return;
}

Program::~Program() {};
