#include "program.hpp"
#include "job_bus.hpp"

#include <thread>

Program::Program(){

    std::thread bus_loop(&JobBus::main_loop);    

    auto skype = []{
       SkypeGui    skype;
       skype.run();
    };

    std::thread skype_loop(skype);     

    bus_loop.join();
    skype_loop.join();
}

Program::~Program() {};
