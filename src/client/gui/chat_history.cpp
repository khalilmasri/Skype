// #include "chat_history.hpp"
// #include "skype_gui.hpp"



// ChatHistory::ChatHistory()
// {
//     //Constructor
// }

// ChatHistory::~ChatHistory() 
// {
//     //Destructor
// }

// void ChatHistory::display_chat_history(Client &t_client)
// {
//     std::string current_contact = t_client.contact_get_current_contact();
//     set_panel(150, 0, 650, 520);
//     auto_scroll(current_contact);

//     ImGui::Begin(current_contact.c_str(), NULL, ImGuiWindowFlags_NoBringToFrontOnFocus);
//     ImGui::TextWrapped("%s", m_chat_history); // chat history panel

//     ImGui::SetScrollHereY(0.999f);
//     ImGui::End();
// }

// void ChatHistory::auto_scroll(std::string &t_current_contact)
// {
//     static std::string prev_contact;
//     static int prev_len;
//     int new_len;

//     if ((new_len = load_chat_history(t_current_contact) > prev_len) && prev_contact == t_current_contact)
//     {
//         ImGui::SetScrollHereY(ImGui::GetScrollY());
//     }
//     else if ((prev_contact != t_current_contact))
//     {
//         ImGui::SetScrollHereY(ImGui::GetScrollY());
//     }
//     prev_len = new_len;
// }

// int ChatHistory::load_chat_history(std::string &t_current_contact)
// {
//     int length = 0;
//     memset(m_chat_history, 0, sizeof(m_chat_history));
//     std::string filename = "../chat_logs/" + t_current_contact + ".txt";
//     std::fstream file;
//     file.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
//     if (!file)
//         return -1;
//     else
//         length = file_to_buffer(file);
//     return length;
// };

// int ChatHistory::file_to_buffer(std::fstream &file)
// {
//     int length;
//     file.seekg(0, file.end);
//     length = file.tellg();
//     file.seekg(0, file.beg);
//     file.read(m_chat_history, length);
//     file.close();
//     return length;
// };

// void ChatHistory::set_panel(int pos_x, int pos_y, int size_x, int size_y)
// {
//     const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
//     ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + pos_x, main_viewport->WorkPos.y + pos_y), ImGuiCond_FirstUseEver);
//     ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_FirstUseEver);
// }

// void ChatHistory::set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag){
//     ImGui::Text("%s", t_field);
//     ImGui::PushItemWidth(t_width);
//     ImGui::InputText(t_label, buf, sizeof(buf), t_flag);
// }
