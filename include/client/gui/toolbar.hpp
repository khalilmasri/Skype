#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "client.hpp"

class ToolBar 
{
    public:
        ToolBar();
        ~ToolBar();

        void display_chat_toolbar(Client &t_client);

    private:

        void set_panel(int pos_x, int pos_y, int size_x, int size_y);
        void set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag);
        //variables

};

#endif