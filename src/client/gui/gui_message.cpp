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


// function to add timestamps and Nametag to chat history, i assume will be called after
// message is recieved before writing to chat history txt file.
// needs <ctime> <chrono> <string> includes
// std::string stamp_message(std::string &t_message, std::string &t_current_contact)
// {
//     std::string stamped_message;
//     std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//     std::tm ltime;
//     time_t ttime = time(0);
    
//     std::string dt = ctime(&ttime);
//     dt = dt.substr(dt.length() - 14, 5);

//     stamped_message = dt + " - " + t_current_contact + "\n" + t_message + "\n\n"; //include empty line
//     return stamped_message;
// }