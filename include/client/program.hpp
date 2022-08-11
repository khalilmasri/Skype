#ifndef PROGRAM_H
#define PROGRAM_H

#include "client.hpp"
#include "skype_gui.hpp"
#include "job_bus.hpp"

class Program{

public:
    Program(int t_port);
    ~Program();

private:
    SkypeGui    m_skype;
    JobBus      m_bus;
};

#endif // PROGRAM_H