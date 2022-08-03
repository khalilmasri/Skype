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
        void im_gui_init();
        void window_init();
        void run();
        void render();
        void shutdown();
        void update(); //not used currently

        //specific skype functionality
        void set_panel(int pos_x, int pos_y, int size_x, int size_y);
        void login_window();
        void new_user_window();
        void contacts_list();
        void load_contacts();
        void add_user(std::string &t_new_username);

        //Chats functionality
        void chat_window(const std::string &t_contact);
        void run_chat_controls(const std::string &t_contact);
        int chat_history_to_buffer();
        void run_call_window();
                
        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        
        ImVec4 clear_color;
        //login bools
        bool logged_in = false;
        bool new_user_login = false;
        bool password_error = false;
        bool username_error = false;
    
        //other bools
        bool video_call;
        bool audio_call;
        bool done;
        bool show_another_window = false;

        //buffers
        char m_username[20];
        char m_password[20];
        char m_confirm_password[20];
        char m_message[MAX_MESSAGE_LENGTH];
        std::vector<std::string> m_contacts;

        //chat variables
        std::string m_current_contact; //to decide which contact was selected last
        char m_chat_history[MAX_HISTORY_LENGTH]; //buffer to write the current contacts chat history into
};