#include "gui_message.hpp"
#include "imgui.h"

// std::string GuiMsg::m_msg;

GuiMsg gui_message;

void GuiMsg::display(){
   ImGui::Text("%s", m_msg.c_str());
}

void GuiMsg::set_msg(const char *t_msg){
   m_msg = t_msg;
}