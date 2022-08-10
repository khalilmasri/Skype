#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "skype_gui.hpp"

#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"

#include "SDL.h"
#include "SDL_opengl.h"

#include <iostream>
#include <string>
#include <vector>

#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "fail_if.hpp"

#include <cstdio>
// // Main code
// int main(int, char **)
// {
//     SkypeGui skype_gui;

//     // Run() executes the loop until 'done'
//     skype_gui.run();

// // client --> call ImGui context initialisation --> then initialise chat windows

// // Main code
int main(int, char **)
{   
    
    Client client(5000);
    std::string name = "";
    std::vector<std::string> contacts = {};
    std::string username_wrong = "khalil";
    std::string password_wrong = "1234";

    FAIL_IF( false == client.user_set_password(password_wrong));
    FAIL_IF( false == client.user_set_username(username_wrong));

    client.user_register_user();

    client.user_login();

    name = "pedro";
    client.contact_search(name);

    std::cout << "=====================================" << std::endl;
    client.contact_add_user(name);

    std::cout << "=====================================" << std::endl;
    client.contact_remove_user(name);
    
    std::cout << "=====================================" << std::endl;
    name = "marcos";
    client.contact_add_user(name);
    
    std::cout << "=====================================" << std::endl;
    name = "pedro";
    client.contact_add_user(name);
    
    std::cout << "=====================================" << std::endl;
    client.contact_list();

    std::cout << "=====================================" << std::endl;
    contacts = client.contact_get_contacts();

    for ( auto &contact : contacts ) {
        std::cout << "Username=" << contact <<std::endl; 
    }

    return 1;

fail:
    return 0;
}

