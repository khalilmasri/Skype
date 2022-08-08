#ifndef LOGINGUI_H
#define LOGINGUI_H

#include "client.hpp"

#define MAX_USER_LEN 20

class LoginGui {

public:
   LoginGui();
   void welcome(Client& t_client);

private:
   void login(Client& t_client);
   void register_window(Client& t_client);
   void register_user(Client& t_client);
   void memset_variables();
   
   bool m_new_user;
   char m_username[MAX_USER_LEN];
   char m_password[MAX_USER_LEN];
   char m_confirm_password[MAX_USER_LEN];

   void set_panel(int pos_x, int pos_y, int size_x, int size_y);
   void set_boxes(const char* t_field, float t_width, const char* t_label, char* buf, ImGuiInputTextFlags t_flag);

};

#endif // LOGINGUI_H