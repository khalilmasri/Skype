#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include "logger.hpp"
#include "skype_gui.hpp"
#include "login_gui.hpp"

LoginGui::LoginGui() 
{
   m_new_user = false;
   memset_variables();
}

void LoginGui::memset_variables()
{
    std::memset(m_username, 0, sizeof(m_username));
    std::memset(m_password, 0, sizeof(m_password));
    std::memset(m_confirm_password, 0, sizeof(m_confirm_password));

}

void LoginGui::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}

void LoginGui::set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag){
    ImGui::Text("%s", t_field);
    ImGui::PushItemWidth(t_width);
    ImGui::InputText(t_label, buf, sizeof(buf), t_flag);
}

void LoginGui::welcome(Client& t_client)
{
    if ( true == m_new_user ) {
        register_window(t_client);
    }

    set_panel(300, 50, 220, 220);
    ImGui::Begin("My_Skype: Login");

    set_boxes("Username", 200, "##usernameField", m_username, ImGuiInputTextFlags_CharsNoBlank);
    set_boxes("Password", 200, "##passwordField", m_password, ImGuiInputTextFlags_Password);

    if (ImGui::Button(" LOGIN "))
    {
        login(t_client);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("NEW USER"))
    {
        m_new_user = true;
    }

    ImGui::End();
}

void LoginGui::register_window(Client& t_client)
{
    set_panel(300, 50, 220, 270);
    ImGui::Begin("My_Skype:New User");

    set_boxes("Username", 200, "##usernameField", m_username, ImGuiInputTextFlags_CharsNoBlank);
    set_boxes("Password", 200, "##passwordField", m_password, ImGuiInputTextFlags_Password);
    set_boxes("Confirm Password", 200, "##confirmpasswordField", m_confirm_password, ImGuiInputTextFlags_Password);

    if (ImGui::Button(" NEW USER LOGIN "))
    {
        register_user(t_client);
    }

    ImGui::End();
}

void LoginGui::login(Client& t_client) {

    std::string password = m_password;
    std::string username = m_username;
    
    t_client.user_set_username(username);
    t_client.user_set_password(password);

    t_client.user_login();

    memset_variables();
}

void LoginGui::register_user(Client& t_client)
{
    
    std::string password = m_password;
    std::string username = m_username;
    std::string confirm_password = m_confirm_password;

    bool res = false;
    
    if ( confirm_password == password ) {
        LOG_INFO("Registering user...");

        t_client.user_set_username(username);
        t_client.user_set_password(password);

        res = t_client.user_register_user();
    }
    else {
        LOG_INFO("Error registering user, check credintials");
        return;
    }

    if ( true == res ) {
        LOG_INFO("Register user was successful");
        m_new_user = false;
    } else {
        LOG_INFO("Error registering user");
    }

    memset_variables();
}
