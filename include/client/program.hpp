#ifndef PROGRAM_H
#define PROGRAM_H

#include "client.hpp"
#include "skype_gui.hpp"

#include <thread>

class Program{

public:
    Program();
    ~Program();

private:
    // SkypeGui    m_skype;
    Client      m_client;
};

#endif // PROGRAM_H
