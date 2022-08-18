#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include "logger.hpp"
#include "login_gui.hpp"
#include "fail_if.hpp"
#include "gui_message.hpp"
#include "job.hpp"
#include "job_bus.hpp"

LoginGui::LoginGui() 
{
   m_new_user = false;
   m_logged = false;

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

void LoginGui::welcome()
{  
    if ( true == m_new_user ) {
        register_window();
        return;
    }

    set_panel(300, 50, 220, 220);
    ImGui::Begin("My_Skype: Login");

    set_boxes("Username", 200, "##usernameField", m_username, ImGuiInputTextFlags_CharsNoBlank);
    set_boxes("Password", 200, "##passwordField", m_password, ImGuiInputTextFlags_Password);

    if (ImGui::Button(" LOGIN "))
    {
        login();
        memset_variables();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("NEW USER"))
    {
        m_new_user = true;
    }  
    GuiMsg::display();

    ImGui::End();

}

void LoginGui::register_window()
{
    set_panel(300, 50, 220, 270);
    ImGui::Begin("My_Skype:New User");

    set_boxes("Username", 200, "##usernameField", m_username, ImGuiInputTextFlags_CharsNoBlank);
    set_boxes("Password", 200, "##passwordField", m_password, ImGuiInputTextFlags_Password);
    set_boxes("Confirm Password", 200, "##confirmpasswordField", m_confirm_password, ImGuiInputTextFlags_Password);

    if (ImGui::Button(" NEW USER LOGIN "))
    {   
        register_user();
        memset_variables();
    }
    GuiMsg::display();

    ImGui::End();
}

void LoginGui::login() {

    std::string password = m_password;
    std::string username = m_username;

    JobBus::handle({Job::SETUSER, username});
    JobBus::handle({Job::SETPASS, password});
    JobBus::handle({Job::LOGIN});

}

void LoginGui::register_user()
{
    std::string password = m_password;
    std::string username = m_username;
    std::string confirm_password = m_confirm_password;
    
    FAIL_IF_GUI(confirm_password != password, "No match password"); 

    JobBus::handle({Job::SETUSER, username});
    JobBus::handle({Job::SETPASS, password});
    JobBus::handle({Job::CREATE});
    
    LOG_INFO("Register user was successful");

fail:
    
    LOG_INFO("Registering user failed!");
    return;
}

bool LoginGui::get_logged(){
    return m_logged;
}

void LoginGui::set_logged(Job &t_job){
    m_logged = t_job.m_valid;
}

void LoginGui::set_new_user(Job &t_job){
    m_new_user = t_job.m_valid;
}