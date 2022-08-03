#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"

#include "SDL.h"
#include "SDL_opengl.h"

#include "my_gui.hpp"

//client --> call ImGui context initialisation --> then initialise chat windows

// Main code
int main(int, char **)
{
    // SkypeGui skype_gui;
    // skype_gui.ImGuiInit();
    // skype_gui.WindowInit();
    
    std::string contact1 = "khalil";
    std::string password = "khalilpass";
    std::string contact2 = "pedro";
    std::string contact3 = "chris";
    Client client(4000);

    std::cout << "User test" << std::endl;
    client.user_set_password(password);
    client.user_set_username(contact1);
    client.user_login();

    std::cout << "User => " << client.user_get_username() << std::endl;
    std::cout << "Logged => " << client.user_get_logged_in() << std::endl;
    
    std::cout << "Contacts" << std::endl;
    
    client.contact_add_user(contact1);
    client.contact_add_user(contact2);
    client.contact_add_user(contact3);
    
    std::cout << "Listing contacts" << std::endl;
    for (auto it : client.contact_get_contacts()){
        std::cout << it << std::endl;
    }

    client.contact_remove_user(contact1);
    std::cout << "Listing contacts after remove" << std::endl;
    for (auto it : client.contact_get_contacts()){
        std::cout << it << std::endl;
    }

    // Events *event = Events::get_instance(Events::SERVER);

    // skype_gui.done = false;
    // //Run() executes the loop until 'done'
    // skype_gui.Run();

    // skype_gui.ShutDown();

    return 0;
}
