#include "user.hpp"
#include "logger.hpp"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

/* Public */

bool User::register_user(int t_socket_fd) {
    
    std::string success_reply = "200";
    std::string reply = "";
    auto pos = 0;

    std::string command = "REGISTER " + m_username + " " + m_password;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos == std::string::npos ) {
        LOG_ERR("Register failed. Server reply => %s", reply);
        return false;
    }

    return true;
}

bool User::login(int t_socket_fd) {

    std::string success_reply = "200";
    std::string reply = "";
    auto pos = 0;

    std::string command = "LOGIN " + m_username + " " + m_password;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos == std::string::npos ) {
        LOG_ERR("Login failed. Server reply => %s", reply);
        return false;
    }

    m_logged_in = true;

    return true;
}

bool User::set_username(std::string& t_username) {

    if ( t_username == "" ){
        LOG_ERR("Invalid username");
        return false;
    }

    m_username = t_username;

    return true;
}

bool User::set_password(std::string& t_password) {

    if ( t_password == "" ) {
        LOG_ERR("Invalid password");
        return false;
    } 

    m_password = t_password;

    return true;
}

std::string User::get_username() const {
    return m_username;
}

bool User::get_logged_in() const {
    return m_logged_in;
}

/* Private */

bool User::handle_command(int t_socket_fd, std::string& t_cmd, std::string& t_reply){

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
