#include "imgui.h"
#include <SDL.h>

class SkypeGui
{
    
    public:

        //main Imgui path
        void ImGuiInit();
        void WindowInit();
        void Run();
        void Render();
        void ShutDown();
        void Update(); //not used currently

        //specific skype functionality
        void LoginWindow();
        void ContactsList();
        void LoadContacts();
        void AddUser(std::string new_username);

        //Chats functionality
        void ChatWindow(std::string contact);
        

        
        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        
        ImVec4 clear_color;
        bool logged_in = false;
        //buffers for the logins
        char username[20];
        char password[20];
        std::vector<std::string> contacts;
    
        //other bools
        bool video_call;
        bool done;
        bool show_another_window = false;
};


