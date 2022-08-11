#include "program.hpp"

#include <thread>

Program::Program(int t_port) : m_skype() , m_bus(t_port) {

    std::thread bus_loop(&JobBus::main_loop);
    
    m_skype.run();
}

Program::~Program() {};