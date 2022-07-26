#include "../include/imgui.h"
#include "../include/imgui_impl_sdl.h"
#include "../include/imgui_impl_opengl2.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include <stdio.h>
#include <iostream>

#include "../include/my_gui.hpp"


void SkypeGui::ImGuiInit(){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::string err = SDL_GetError();
        std::cout << "Error: " << err << std::endl;
    
    }
    std::cout << "SDL initialization completed" << std::endl;

     // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("My_Skype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 600, window_flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    std::cout << "ImGui Initialized" << std::endl;
};

void SkypeGui::WindowInit()
{
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    io.Fonts->AddFontFromFileTTF("include/Cousine-Regular.ttf", 20.0f);
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //some flags maybe we need
    video_call = false;
};

void SkypeGui::Run()
{  
    while(!this->done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                this->done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                this->done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if(!logged_in)
        {
            LoginWindow();
        }   

        // // 3. Show another simple window.
        // if (show_another_window)
        // {
        //     ImGui::Begin("Another Chat", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //     ImGui::Text("Hello from another Chat Contact!");
        //     if (ImGui::Button("Close Me"))
        //         show_another_window = false;
        //     ImGui::End();
        // }

        Render();

    }

}

void SkypeGui::LoginWindow()
{
    ImGui::Begin("My_Skype: Login");
    ImGui::PushItemWidth(100);
    ImVec2 Alignment(ImGui::GetWindowSize().x / 2 - (ImGui::CalcItemWidth() / 2), 50);	//Align at center of window with respect to itemWidth
    ImGui::Indent(Alignment.x);
    ImGui::SetCursorPosY(Alignment.y);

    ImGui::Text("Username");
    ImGui::InputText("##usernameField", username, sizeof(username), ImGuiInputTextFlags_CharsNoBlank);
    
    ImGui::Text("Password");
    ImGui::InputText("##passwordField", password, sizeof(password), ImGuiInputTextFlags_Password);

    if(ImGui::Button("LOG IN"))
    {
        std::cout << "User Logged Into Skype" << std::endl;
        logged_in = true;
    }
    ImGui::End();
}

void SkypeGui::Update()
{
   
};

void SkypeGui::Render()
{
    // Rendering

        ImGuiIO &io = ImGui::GetIO();
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        // glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
};

void SkypeGui::ShutDown(){};

