#ifndef CHAT_HISTORY_H
#define CHAT_HISTORY_H

#include "imgui.h"
#include "client.hpp"

#define MAX_HISTORY_LENGTH 2048 //how do i include the define from somewhere else with includes?

class ChatHistory
{
    public:
        ChatHistory();
        ~ChatHistory();

        void display_chat_history(Client &t_client);

    private:
        int load_chat_history(std::string &t_current_contact);
        int file_to_buffer(std::fstream &file);
        void auto_scroll(std::string &t_current_contact);

        void set_panel(int pos_x, int pos_y, int size_x, int size_y);
        void set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag);

        //buffer
        char m_chat_history[MAX_HISTORY_LENGTH];


    
};


#endif