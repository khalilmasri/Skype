#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "skype_gui.hpp"

// Main code
int main(int, char **)
{
    SkypeGui skype_gui;

    // Run() executes the loop until 'done'
    skype_gui.run();

    return 0;
}
