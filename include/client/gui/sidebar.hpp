#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "job.hpp"

#include <string>
#include <vector>

class SideBar {
//TO DO 
    public:
        SideBar();

        //main method to display sidebar
        void display_sidebar();
        void set_contact_list(Job &t_job); 

        std::string get_current_user();

    private:

        // setup and fill contacts list
        void contacts_sidebar();
        void make_list_box(std::string &t_list_name, int x_size, int y_size);
        void selectable_list();

        void set_panel(int pos_x, int pos_y, int size_x, int size_y);

        std::vector<std::string> m_contacts;
        std::string m_current_user;

};

#endif // SIDEBAR_H