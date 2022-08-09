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

#include <iostream>
#include <string>
#include <vector>

#include "client.hpp"
#include "user.hpp"
#include "contacts.hpp"
#include "fail_if.hpp"

#include <cstdio>
// client --> call ImGui context initialisation --> then initialise chat windows

// Main code
int main(int, char **)
{   
    
    Client client(5000);
    std::string name = "";
    std::string username_wrong = "khalil";
    std::string password_wrong = "1234";

    FAIL_IF( false == client.user_set_password(password_wrong));
    FAIL_IF( false == client.user_set_username(username_wrong));

    // client.user_register_user();

    client.user_login();

    name = "khalil";

    client.contact_search(name);
    client.contact_add_user(name);
    client.contact_list();
// // while(true){
//     sleep(5);
// }
    return 1;

fail:
    return 0;
}
