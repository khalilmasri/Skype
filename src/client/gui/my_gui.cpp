#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL.h"
#include "SDL_opengl.h"

#include "my_gui.hpp"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

void SkypeGui::im_gui_init()
{
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
    window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI); // SDL_WINDOW_RESIZABLE);
    window = SDL_CreateWindow("My_Skype", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
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

    // Setup Dear ImGui style (comment/uncomment)
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    std::cout << "ImGui Initialized" << std::endl;
};

void SkypeGui::window_init()
{
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    io.Fonts->AddFontFromFileTTF("../misc/Cousine-Regular.ttf", 20.0f);
    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // maybe video call flag to trigger panel for video
    video_call = false;
};

void SkypeGui::run()
{
    load_contacts();
    while (!this->done)
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

        if (!logged_in && !new_user_login)
        {
            login_window();
        }
        else if (new_user_login)
        {
            new_user_window();
        }
        else if (logged_in)
        {
            contacts_list();
            if (m_current_contact != "") // only after a contact is selected
            {
                chat_window(m_current_contact);
                run_chat_controls(m_current_contact);
                if (video_call || audio_call)
                    run_call_window();
            }
        }
        render();
    }
}

void SkypeGui::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}

void SkypeGui::login_window()
{
    set_panel(300, 50, 220, 220);
    ImGui::Begin("My_Skype: Login");

    ImGui::Text("Username");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##usernameField", m_username, sizeof(m_username), ImGuiInputTextFlags_CharsNoBlank);

    ImGui::Text("Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##passwordField", m_password, sizeof(m_password), ImGuiInputTextFlags_Password);

    // conditions for login here necessary
    if (ImGui::Button(" LOGIN "))
    {
        std::cout << "User Logged Into Skype" << std::endl;
        logged_in = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("NEW USER"))
    {
        std::cout << "New User Window" << std::endl;
        new_user_login = true;
    }
    if (username_error)
        ImGui::Text("Username Not Found");
    if (!username_error && password_error)
        ImGui::Text("Incorrect Password");
    ImGui::End();
}

void SkypeGui::new_user_window()
{
    set_panel(300, 50, 220, 270);
    // window size is longer to account for both 2 separate error messages possible simultaneously
    ImGui::Begin("My_Skype:New User");

    ImGui::Text("Username");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##usernameField", m_username, sizeof(m_username), ImGuiInputTextFlags_CharsNoBlank);

    ImGui::Text("New Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##passwordField", m_password, sizeof(m_password), ImGuiInputTextFlags_Password);

    ImGui::Text("Confirm Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##confirmpasswordField", m_confirm_password, sizeof(m_confirm_password), ImGuiInputTextFlags_Password);

    if (ImGui::Button(" NEW USER LOGIN "))
    {

        // need also condition for username available/taken with bool username_error
        if ((strcmp(m_confirm_password, m_password) == 0) && (strcmp(m_password, "\0") != 0))
        {
            std::cout << "New User Successfully Logged in" << std::endl;
            logged_in = true;
            new_user_login = false;
            password_error = false;
        }
        else
        {
            password_error = true;
            memset(m_confirm_password, '\0', sizeof(m_confirm_password));
            memset(m_password, '\0', sizeof(m_password));
        }
    }
    // window sized to account for only one of these errors at a time
    // i.e if username doesnt exist then that displays first
    if (password_error)
        ImGui::Text("Incorrect Password");
    if (username_error)
        ImGui::Text("Username Taken");

    ImGui::End();
}

void SkypeGui::contacts_list()
{
    set_panel(0, 0, 150, 600);
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(150, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Contacts");
    ImGui::PushItemWidth(300);

    static int index = 0; // Here we store our selection data as an index.
    if (ImGui::ListBoxHeader("##ContactList", ImVec2(150, 550)))
    {
        for (size_t n = 0; n < m_contacts.size(); n++)
        {
            const bool is_selected = (index == (int)n);
            if (ImGui::Selectable(m_contacts[n].c_str(), is_selected) && video_call == false && audio_call == false) // block changing chats if on call
            {
                index = n;
                std::cout << "selected: " << m_contacts[n] << std::endl;
                m_current_contact = m_contacts[n];
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
    ImGui::End();
};

void SkypeGui::load_contacts()
{
    // manual for now, will reimplement when we figure how we are planning to store users
    m_contacts.push_back("Khalil");
    m_contacts.push_back("Pedro");
    m_contacts.push_back("Chris");
}

void SkypeGui::add_user(std::string &t_new_username)
{
    // to add a new user while the program is running to simulate new user entering
    m_contacts.push_back(t_new_username);
    std::cout << "User Added: " << t_new_username << std::endl;
}

void SkypeGui::chat_window(const std::string &t_contact)
{
    static std::string prev_contact;
    static int prev_len;
    int new_len;
    if ((new_len = chat_history_to_buffer() > prev_len) && prev_contact == t_contact)
    {
        ImGui::SetScrollHereY(ImGui::GetScrollY());
    }
    else if ((prev_contact != t_contact))
    {
        ImGui::SetScrollHereY(ImGui::GetScrollY());
    }
    else
    {
        prev_len = new_len;
    }
    prev_len = new_len;

    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 150, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(650, 520), ImGuiCond_FirstUseEver);

    ImGui::Begin(t_contact.c_str(), NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::TextWrapped("%s", m_chat_history); // chat history panel

    ImGui::SetScrollHereY(0.999f);
    ImGui::End();
};

void SkypeGui::run_chat_controls(const std::string &t_contact)
{
    set_panel(150, 520, 650, 80);

    ImGui::Begin("##MessagePanel", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGuiInputTextFlags enter_pressed = ImGuiInputTextFlags_EnterReturnsTrue;
    ImGui::PushItemWidth(440);
    if (ImGui::InputText("##ChatBox", m_message, 1000, enter_pressed))
    {
        ImGui::SetItemDefaultFocus();
        if (enter_pressed)
        {
            std::cout << "User Sent Message: " << m_message << std::endl;
            memset(m_message, '\0', strlen(m_message));
            ImGui::SetKeyboardFocusHere(-1); // returns focus to InputText always
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("SEND"))
    {
        std::cout << "User Sent Message: " << m_message << " to " << t_contact << std::endl;
        memset(m_message, '\0', strlen(m_message));
    }
    ImGui::SameLine();
    if (ImGui::Button("CALL"))
    {
        if (audio_call == false)
        {
            audio_call = true;
            std::cout << "User Requested Audio Call" << m_message << std::endl;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("VIDEO"))
    {
        if (video_call == false)
        {
            video_call = true;
            std::cout << "User Requested Video Call" << m_message << std::endl;
        }
        audio_call = true;
    }

    ImGui::End();
};

int SkypeGui::chat_history_to_buffer()
{
    int length = 0;
    memset(m_chat_history, 0, sizeof(m_chat_history));
    std::string filename = "../chat_logs/" + m_current_contact + ".txt";
    std::fstream file;
    file.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!file)
    {
        std::cout << "Chat History Could Not Be Found!" << std::endl;
    }
    else
    {
        file.seekg(0, file.end);
        length = file.tellg();
        file.seekg(0, file.beg);
        file.read(m_chat_history, length);
        file.close();
    }
    return length;
};

void SkypeGui::run_call_window()
{
    set_panel(500, 0, 300, 300);

    std::string str = "Call with " + m_current_contact;
    ImGui::Begin(str.c_str());

    static int volume = 0;
    ImGui::VSliderInt("##volume_slider", ImVec2(20, 250), &volume, 0, 9);
    ImGui::SameLine();

    // Put video stream here;
    if (ImGui::Button("CLOSE"))
    {
        video_call = false;
        audio_call = false;
        std::cout << "Close Call" << std::endl; // close video stream and close window!
    }
    ImGui::SameLine();
    if (ImGui::Button("VIDEO"))
    {
        if (video_call == false)
        {
            video_call = true;
            std::cout << "Open Video Stream" << std::endl;
        }
        else
        {
            video_call = false;
            std::cout << "Close Video Stream" << std::endl;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("MUTE"))
    {
        std::cout << "User Muted/Unmuted" << std::endl; // Code to Mute, either cut the audio completely or just stop sending...
    }
    ImGui::End();
}

void SkypeGui::render()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Render();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
};

void SkypeGui::shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    std::cout << "ImGui Context Destroyed" << std::endl;

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "SDL Quit" << std::endl;
};