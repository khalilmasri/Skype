#include "../include/imgui.h"
#include "../include/imgui_impl_sdl.h"
#include "../include/imgui_impl_opengl2.h"
#include <stdio.h>

#include "SDL.h"
#include "SDL_opengl.h"

#include "../include/my_gui.hpp"

//client --> call ImGui context initialisation --> then initialise chat windows

// Main code
int main(int, char **)
{
    SkypeGui skype_gui;
    skype_gui.ImGuiInit();
    skype_gui.WindowInit();
   

    skype_gui.done = false;
    //Run() executes the loop until 'done'
    skype_gui.Run();

    skype_gui.ShutDown();

    return 0;
}
