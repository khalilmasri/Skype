#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "skype_gui.hpp"


// client --> call ImGui context initialisation --> then initialise chat windows

// Main code
int main(int, char **)
{
    SkypeGui skype_gui;

    // Run() executes the loop until 'done'
    skype_gui.run();

    return 0;
}
