#include "sidebar.hpp"
#include "skype_gui.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "fail_if.hpp"

#include <string>

SideBar::SideBar() 
{
    std::cout << "Sidebar Constructed" << std::endl;
}

SideBar::~SideBar()
{
    std::cout << "Sidebar Destructed" << std::endl;
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

void SideBar::make_list_box(std::string &t_list_name, int x_size, int y_size)
{
    if (ImGui::ListBoxHeader(t_list_name.c_str(), ImVec2(150, 550)))
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
    Job job;
    std::vector<std::string> contacts;

    job = {Job::DISP_CONTACTS};
    FAIL_IF_MSG( false == JobBus::handle(job), "Couldn't load contacts");
    contacts = job.m_vector;

    for (; n < contacts.size(); n++)
    {
        is_selected = (index == (int)n);
        // TEMP booleans needed toblock changing chats if on call
        if (ImGui::Selectable(contacts[n].c_str(), is_selected)) // && video_call == false && audio_call == false)
        {
            index = n;
            FAIL_IF_MSG( false == JobBus::handle({Job::SELCONT, contacts[n]}), "Couldn't select contact");
        }
        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
        {
            ImGui::SetItemDefaultFocus();
        }
    }

fail:
    return;
}

void SideBar::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}
