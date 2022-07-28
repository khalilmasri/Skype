#include "contacts.hpp"
#include "logger.hpp"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

/* Public */

bool Contacts::list() {

    
}

bool Contacts::handle_command(int t_socket_fd, std::string& t_cmd, std::string& t_reply){

    int pos;

    char msg[MAX_MSG_LEN];
    std::memset(&msg, 0 ,sizeof(msg));
    
    std::strcpy(msg, t_cmd.c_str());

    int res = send(t_socket_fd, (char*)msg, std::strlen(msg), 0);
    if( -1 == res ){
        LOG_ERR("Wrong command, or arguments");
        return false;
    }

    std::memset(&msg, 0 ,sizeof(msg));
    res = recv(t_socket_fd, (char*)&msg, MAX_MSG_LEN, 0);
    if( -1 == res ){
        LOG_ERR("Server reply => %s", msg);
        return false;
    }

    t_reply = msg;

    return true;
}