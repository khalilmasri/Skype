#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <string>

class SideBar {
//TO DO 
    public:
        SideBar();
        ~SideBar();
        //main method to display sidebar
        void display_sidebar();


    private:

        // setup and fill contacts list
        void contacts_sidebar();
        void make_list_box(std::string &t_list_name, int x_size, int y_size);
        void selectable_list();

        void set_panel(int pos_x, int pos_y, int size_x, int size_y);

};

#endif // SIDEBAR_H