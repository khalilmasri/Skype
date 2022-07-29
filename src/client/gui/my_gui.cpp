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

void SkypeGui::ImGuiInit()
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

void SkypeGui::WindowInit()
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

void SkypeGui::Run()
{
    int i = 0;
    LoadContacts();
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

        if (!logged_in && !new_user)
        {
            LoginWindow();
        }
        else if (new_user)
        {
            NewUser();
        }
        else
        {
            // Skype platform from here
            i++;
            ContactsList();
            if (i == 400) // simulating new users coming on
            {
                AddUser("Johnny");
            }
            if (i == 800)
            {
                AddUser("BigChungus");
            }
            if (current_contact != "")
                ChatWindow(current_contact);
                RunChatControls(current_contact);
            if (video_call)
                RunVideoWindow();
        }

        Render();
    }
}

void SkypeGui::LoginWindow()
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 300, main_viewport->WorkPos.y + 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(220, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("My_Skype: Login");

    ImGui::Text("Username");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##usernameField", username, sizeof(username), ImGuiInputTextFlags_CharsNoBlank);

    ImGui::Text("Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##passwordField", password, sizeof(password), ImGuiInputTextFlags_Password);

    //conditions for login here necessary
    if (ImGui::Button(" LOGIN "))
    {
        std::cout << "User Logged Into Skype" << std::endl;
        logged_in = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("NEW USER"))
    {
        std::cout << "New User Window" << std::endl;
        new_user = true;
    }
    if (username_error)
        ImGui::Text("Username Not Found");
    if (!username_error && password_error)
        ImGui::Text("Incorrect Password");
    ImGui::End();
}

void SkypeGui::NewUser()
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 300, main_viewport->WorkPos.y + 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(220, 270), ImGuiCond_FirstUseEver);
    //window size is longer to account for both 2 separate error messages possible simultaneously
    ImGui::Begin("My_Skype:New User");

    ImGui::Text("Username");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##usernameField", username, sizeof(username), ImGuiInputTextFlags_CharsNoBlank);

    ImGui::Text("New Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##passwordField", password, sizeof(password), ImGuiInputTextFlags_Password);

    ImGui::Text("Confirm Password");
    ImGui::PushItemWidth(200);
    ImGui::InputText("##confirmpasswordField", confirm_password, sizeof(confirm_password), ImGuiInputTextFlags_Password);

    if (ImGui::Button(" NEW USER LOGIN "))
    {

        //need also condition for username available/taken with bool username_error
        if ((strcmp(confirm_password, password) == 0) && (strcmp(password, "\0") != 0))
        {
            std::cout << "New User Successfully Logged in" << std::endl;
            logged_in = true;
            new_user = false;
            password_error = false;
        }
        else
        {
            password_error = true;
            memset(confirm_password, '\0', sizeof(confirm_password));
            memset(password, '\0', sizeof(password));
        }
    }
    //window sized to account for only one of these errors at a time
    //i.e if username doesnt exist then that displays first
    if (password_error)
        ImGui::Text("Incorrect Password");
    if (username_error)
        ImGui::Text("Username Taken");

    ImGui::End();
}

void SkypeGui::Update(){

};

void SkypeGui::ContactsList()
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(150, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Contacts");
    ImGui::PushItemWidth(300);

    static int item_current_idx = 0; // Here we store our selection data as an index.
    if (ImGui::ListBoxHeader("##ContactList", ImVec2(150, 550)))
    {
        for (size_t n = 0; n < contacts.size(); n++)
        {
            const bool is_selected = (item_current_idx == (int)n);
            if (ImGui::Selectable(contacts[n].c_str(), is_selected))
            {
                item_current_idx = n;
                std::cout << "selected: " << contacts[n] << std::endl;
                current_contact = contacts[n];
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
    ImGui::End();
};

void SkypeGui::LoadContacts()
{
    // manual for now, will reimplement when we figure how we are planning to store users
    contacts.push_back("Khalil");
    contacts.push_back("Pedro");
    contacts.push_back("Chris");
}

void SkypeGui::AddUser(std::string new_username)
{
    // to add a new user while the program is running to simulate new user entering
    contacts.push_back(new_username);
    std::cout << "User Added: " << new_username << std::endl;
}

void SkypeGui::ChatWindow(const std::string contact)
{
    static std::string prev_contact;
    static int prev_len;
    int new_len;
    if((new_len = ChatHistoryToBuffer() > prev_len) && prev_contact == contact)
    {
        ImGui::SetScrollHereY(ImGui::GetScrollY());
    }
    else if ((prev_contact != contact))
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
    ImGui::SetNextWindowSize(ImVec2(650, 540), ImGuiCond_FirstUseEver);

    ImGui::Begin(contact.c_str(),NULL , ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::TextWrapped("%s", chat_history);  //chat history panel

    ImGui::SetScrollHereY(0.999f);
    ImGui::End();
};

void SkypeGui::RunChatControls(const std::string contact)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 150, main_viewport->WorkPos.y + 520), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(650, 80), ImGuiCond_FirstUseEver);

    ImGui::Begin("##MessagePanel", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGuiInputTextFlags enter_pressed = ImGuiInputTextFlags_EnterReturnsTrue;
    ImGui::PushItemWidth(440);
    if(ImGui::InputText("##ChatBox", message, 1000, enter_pressed)) 
    {  
        ImGui::SetItemDefaultFocus(); 
        if(enter_pressed)
        {
            std::cout << "User Sent Message: " << message << std::endl;
            memset(message, '\0', strlen(message));
            ImGui::SetKeyboardFocusHere(-1); //returns focus to InputText always
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("SEND"))
    {
        std::cout << "User Sent Message: " << message << " to " << contact << std::endl;
        memset(message, '\0', strlen(message));
    }
    ImGui::SameLine();
    if (ImGui::Button("CALL"))
    {
        audio_call = true;
        std::cout << "User Requested Audio Call" << message << std::endl;
    }
    ImGui::SameLine();
    if (ImGui::Button("VIDEO"))
    {
        video_call = true;
        audio_call = true;
        std::cout << "User Requested Video Call" << message << std::endl;
    }

    ImGui::End();
};


int SkypeGui::ChatHistoryToBuffer()
{
    int length = 0;
    memset(chat_history, 0, sizeof(chat_history));
    std::string filename = "../chat_logs/" + current_contact + ".txt";
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
        file.read(chat_history, length);
        file.close();
    }
    return length;
    
};

void SkypeGui::RunVideoWindow()
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 500, main_viewport->WorkPos.y + 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    std::string str = "Video Call with " + current_contact;
    ImGui::Begin(str.c_str());
        //Put video stream here;
        if(ImGui::Button("CLOSE VIDEO"))
        {
            video_call = false;
            //close video stream and close window!
            std::cout << "Close Video Stream" << std::endl;
        }
    ImGui::End();

}



void SkypeGui::Render()
{
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Render();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
};

void SkypeGui::ShutDown()
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