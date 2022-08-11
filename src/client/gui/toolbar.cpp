#include "skype_gui.hpp"
#include "toolbar.hpp"

ToolBar::ToolBar()
{
    //Constructor
}

ToolBar::~ToolBar()
{
    //Destructor
}

void ToolBar::display_chat_toolbar(Client &t_client)
{
    std::cout << "TODO: Finish the toolbar class" << std::endl;
}

void ToolBar::set_panel(int pos_x, int pos_y, int size_x, int size_y)
{
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
}

void ToolBar::set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag){
    ImGui::Text("%s", t_field);
    ImGui::PushItemWidth(t_width);
    ImGui::InputText(t_label, buf, sizeof(buf), t_flag);
}