#ifndef PROGRAM_H
#define PROGRAM_H

#include "client.hpp"
#include "skype_gui.hpp"
#include "job_bus.hpp"

#include <thread>

class Program{

public:
    Program();
    ~Program();

private:
    SkypeGui    m_skype;
};

#endif // PROGRAM_H
