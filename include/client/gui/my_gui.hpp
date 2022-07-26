#include "imgui.h"
#include <SDL.h>

class SkypeGui
{
    
    public:

        void ImGuiInit();
        void WindowInit();
        void Run();
        void LoginWindow();
        virtual void Update();
        void Render();
        void ShutDown();

        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        
        ImVec4 clear_color;
        bool logged_in = false;
        //buffers for the logins
        char username[20];
        char password[20];

        //other bools
        bool video_call;
        bool done;
        bool show_another_window = false;
};


