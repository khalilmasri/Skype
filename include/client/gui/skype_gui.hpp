#include "imgui.h"
#include "client.hpp"
#include <SDL.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>

#define MAX_MESSAGE_LENGTH 1024
#define MAX_HISTORY_LENGTH 2048
#define MAX_USER_LEN 20

class SkypeGui
{
    public:

        SkypeGui();
        ~SkypeGui();

        //main Imgui path
       void run();
    
        //other bools
        bool video_call;
        bool audio_call;
        bool done;
        bool show_another_window = false;

        char m_message[MAX_MESSAGE_LENGTH];
        std::vector<std::string> m_contacts;

        //chat variables
        std::string m_current_contact; //to decide which contact was selected last
        char m_chat_history[MAX_HISTORY_LENGTH]; //buffer to write the current contacts chat history into

private:
    bool m_exit;
    bool check_exit(SDL_Event& t_event);

    void im_gui_init();
    void window_init();
    void set_attributes();
    void render();
    void shutdown();
    void update(); //not used currently

    //Chats functionality
    void chat_window(const std::string &t_contact);
    void run_chat_controls(const std::string &t_contact);
    int chat_history_to_buffer();
    void run_call_window();
      
    SDL_WindowFlags m_window_flags;
    SDL_Window* m_window;
    SDL_GLContext m_gl_context;
    ImVec4 m_clear_color;

    //specific skype functionality
    void set_panel(int pos_x, int pos_y, int size_x, int size_y);
    void set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag);

    void contacts_list();

    Client m_client;
};