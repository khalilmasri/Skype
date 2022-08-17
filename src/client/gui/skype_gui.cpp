#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include "logger.hpp"
#include "SDL.h"
#include "SDL_opengl.h"
#include "skype_gui.hpp"
#include "login_gui.hpp"
#include "sidebar.hpp"
#include "chat_history.hpp"
#include "job_bus.hpp"
#include "job.hpp"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

time_t now;

SkypeGui::SkypeGui(){
    im_gui_init();
    window_init();

    m_exit = false;
}

SkypeGui::~SkypeGui(){
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

    LOG_DEBUG("Shuting down client");
    JobBus::set_exit();
}

bool SkypeGui::check_exit(SDL_Event& t_event){
    if (t_event.type == SDL_QUIT)
    {
        return true;
    }

    if ((t_event.type == SDL_WINDOWEVENT) && (t_event.window.event == SDL_WINDOWEVENT_CLOSE) && (t_event.window.windowID == SDL_GetWindowID(m_window)))
    {
        return true;
    }

    return false;
}

void SkypeGui::set_attributes() {

    // Setup m_window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    m_window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI); // SDL_WINDOW_RESIZABLE);
    m_window = SDL_CreateWindow("My_Skype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, m_window_flags);
    m_gl_context = SDL_GL_CreateContext(m_window);
    SDL_GL_MakeCurrent(m_window, m_gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
}

void SkypeGui::im_gui_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::string err = SDL_GetError();
        LOG_ERR("SDL error => %s", err.c_str());
    }
    
    LOG_DEBUG("SDL initialization completed");

    set_attributes();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    LOG_DEBUG("ImGui Initialized")
};

void SkypeGui::window_init()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_gl_context);
    ImGui_ImplOpenGL2_Init();

    io.Fonts->AddFontFromFileTTF("../misc/Cousine-Regular.ttf", 20.0f);
    m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

};

void SkypeGui::run()
{   
    welcome();

    SideBar sidebar;
    time(&now);

    while ( false == m_exit )
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            m_exit = check_exit(event);
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        sidebar.display_sidebar(); //display contacts list 

        render();
    }

}

void SkypeGui::welcome(){
    
    LoginGui login_window;
    bool logged_in = false;

    while ( false == m_exit )
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            m_exit = check_exit(event);
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if ( true == logged_in ) {
            break;
        }
        
        login_window.welcome();
        logged_in = login_window.get_logged();
    
        render();
    }

    render();
}

void SkypeGui::render()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Render();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
};

void SkypeGui::repeat_job(){

    if (difftime(time(NULL), now) > 3){ // Run this task every 3 seconds
        JobBus::handle({Job::LIST});
        time(&now);
    }  
}

void SkypeGui::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}

void SkypeGui::set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag){
    ImGui::Text("%s", t_field);
    ImGui::PushItemWidth(t_width);
    ImGui::InputText(t_label, buf, sizeof(buf), t_flag);
}
