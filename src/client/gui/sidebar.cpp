#include "sidebar.hpp"
#include "skype_gui.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "fail_if.hpp"

#include <string>

SideBar::SideBar() {
}

void SideBar::display_sidebar()
{
    contacts_sidebar();
}

void SideBar::contacts_sidebar()
{
    set_panel(0, 0, 150, 600);
    ImGui::Begin("Contacts");
    ImGui::PushItemWidth(300);
    std::string list_name = "##Contacts";
    make_list_box(list_name, 150, 550);
    ImGui::End();
};

void SideBar::make_list_box(std::string &t_list_name, int t_x_size, int t_y_size)
{
    if (ImGui::ListBoxHeader(t_list_name.c_str(), ImVec2(t_x_size, t_y_size)))
    {
        selectable_list();
        ImGui::EndListBox();
    }
}

void SideBar::selectable_list()
{
    static int index = 0;
    bool is_selected = false;
    size_t n = 0;

    for (; n < m_contacts.size(); n++)
    {
        is_selected = (index == (int)n);
        
        // TEMP booleans needed toblock changing chats if on call
        if (ImGui::Selectable(m_contacts[n].c_str(), is_selected)) // && video_call == false && audio_call == false)
        {
            index = n;
            JobBus::handle({Job::SELCONT, m_contacts[n]});
            m_current_user = m_contacts[n];
        }
        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
        {
            ImGui::SetItemDefaultFocus();
        }
    }
}

void SideBar::set_contact_list(Job &t_job){
    m_contacts = std::move(t_job.m_vector);
}

std::string SideBar::get_current_user(){
    return m_current_user;
}


void SideBar::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}
