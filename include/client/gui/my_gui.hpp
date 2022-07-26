#include "imgui.h"
#include <SDL.h>

class SkypeGui
{
    
    public:

        void ImGuiInit();
        void WindowInit();
        void Run();
        virtual void Update();
        void Render();
        void ShutDown();

        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        ImVec4 clear_color;
        int display_size_x;
        int display_size_y;
        bool video_call;
        bool done;
        bool show_another_window = false;
};


