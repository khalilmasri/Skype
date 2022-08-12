#ifndef GUI_MESSAGE_H
#define GUI_MESSAGE_H

#define MAX_MSG_LEN 1024

#include <string>

struct GuiMsg{

   static void display();
   static void set_msg(const char *t_msg);

   inline static std::string m_msg = "";

};

#endif // GUI_MESSAGE_H