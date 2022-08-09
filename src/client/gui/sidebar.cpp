#include "sidebar.hpp"
#include "skype_gui.hpp"


SideBar::SideBar() 
{
    std::cout << "Sidebar Constructed" << std::endl;
}

SideBar::~SideBar()
{
    std::cout << "Sidebar Destructed" << std::endl;
}

void SideBar::display_sidebar(Client &t_client)
{
    contacts_sidebar(t_client);
}

void SideBar::contacts_sidebar(Client &t_client)
{
    set_panel(0, 0, 150, 600);
    ImGui::Begin("Contacts");
    ImGui::PushItemWidth(300);
    std::string list_name;
    make_list_box(list_name, t_client, 150, 550);
    ImGui::End();
};

void SideBar::make_list_box(std::string &t_list_name, Client &t_client, int x_size, int y_size)
{
    std::vector<std::string> contacts = t_client.contact_get_contacts();
    static int index = 0; // Here we store our selection data as an index.
    if (ImGui::ListBoxHeader(t_list_name.c_str(), ImVec2(150, 550)))
    {
        for (size_t n = 0; n < contacts.size(); n++)
        {
            const bool is_selected = (index == (int)n);
            // TEMP booleans needed toblock changing chats if on call
            if (ImGui::Selectable(contacts[n].c_str(), is_selected)) // && video_call == false && audio_call == false)
            {
                index = n;
                std::cout << "selected: " << contacts[n] << std::endl;
                t_client.contact_set_current_contact(contacts[n]); // need to write getters and setters for this
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void SideBar::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}
