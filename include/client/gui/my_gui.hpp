#include "imgui.h"
#include <SDL.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>

#define MAX_MESSAGE_LENGTH 1000
#define MAX_HISTORY_LENGTH 2048

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
        void ChatWindow(const std::string contact);
        void ChatHistoryToBuffer();
        

        
        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        
        ImVec4 clear_color;
        bool logged_in = false;
        //buffers
        char username[20];
        char password[20];
        char message[MAX_MESSAGE_LENGTH];
        std::vector<std::string> contacts;
    
        //other bools
        bool video_call;
        bool done;
        bool show_another_window = false;

        //chat variables
        std::string current_contact; //to decide which contact was selected last
        char chat_history[MAX_HISTORY_LENGTH]; //buffer to write the current contacts chat history into
};