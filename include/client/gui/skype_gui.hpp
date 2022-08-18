#include "imgui.h"
#include "client.hpp"
#include "login_gui.hpp"
#include "sidebar.hpp"

#include <SDL.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <functional>

#define MAX_MESSAGE_LENGTH 1024
#define MAX_USER_LEN 20

class SkypeGui
{
    typedef Job::Type Type;
    typedef std::function<void (Job &t_job)> SetMethod;
    typedef std::unordered_map<Type, SetMethod> JobDispatch;

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

private:
    bool m_exit;
    bool check_exit(SDL_Event& t_event);

    void im_gui_init();
    void window_init();
    void set_attributes();
    void render();
    void shutdown();
    void update(); //not used currently
    void welcome();
    void repeat_job();
    void job_dispatch();
 
    SDL_WindowFlags m_window_flags;
    SDL_Window* m_window;
    SDL_GLContext m_gl_context;
    ImVec4 m_clear_color;

    //Gui parts
    LoginGui m_login;
    SideBar m_sidebar;

    //specific skype functionality
    void set_panel(int pos_x, int pos_y, int size_x, int size_y);
    void set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag);

    JobDispatch m_map;
};