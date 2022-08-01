#include "contacts.hpp"
#include "logger.hpp"

#include <string>
#include <sys/types.h>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>

/* Public */

bool Contacts::list(int t_socket_fd) {
 
    std::string success_reply = "201";
    std::string reply = "";
    size_t pos = 0;

    std::string command = "LIST";

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos != std::string::npos ) {
        std::cout << reply << std::endl;
        return true;  
    }

    LOG_ERR("List failed. Server reply => %s", reply.c_str());
    return false;
}

bool Contacts::search(int t_socket_fd, std::string& t_cmd){
    
    std::string success_reply = "201";
    std::string reply = "";
    size_t pos = 0;

    std::string command = "SEARCH " + t_cmd;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos != std::string::npos ) {
        std::cout << reply << std::endl;
        return true;  
    }
    
    LOG_ERR("Search %s failed. Server reply => %s",t_cmd.c_str(), reply.c_str());
    return false;
}

bool Contacts::add_user(int t_socket_fd, std::string& t_cmd){

    std::string success_reply = "200";
    std::string reply = "";
    size_t pos = 0;

    std::string command = "ADDUSER " + t_cmd;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos != std::string::npos ) {
        m_contacts.push_back(t_cmd);
        std::cout << reply << std::endl;
        return true;  
    }

    LOG_ERR("Add %s failed. Server reply => %s", t_cmd.c_str(), reply.c_str());
    return false;
}

bool Contacts::remove_user(int t_socket_fd, std::string& t_cmd){

    std::string success_reply = "200";
    std::string reply = "";
    size_t pos = 0;

    std::string command = "REMOVE " + t_cmd;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos != std::string::npos ) {
        std::cout << reply << std::endl;
        return true;  
    }

    auto res = std::find(m_contacts.begin(), m_contacts.end(), t_cmd);
    if( res != m_contacts.end()){
        m_contacts.erase(res);
    }

    LOG_ERR("Remove %s failed. Server reply => %s", t_cmd.c_str(), reply.c_str());
    return false;
}


bool Contacts::available(int t_socket_fd, std::string& t_cmd){

    std::string success_reply = "200";
    std::string reply = "";
    size_t pos = 0;

    std::string command = "REMOVE " + t_cmd;

    handle_command(t_socket_fd, command, reply);

    pos = reply.find(success_reply);
    if( pos != std::string::npos ) {
        std::cout << reply << std::endl;
        return true;  
    }

    LOG_ERR("Checking availabled failed. Server reply => %s", reply.c_str());
    return false;
}   

std::vector<std::string> Contacts::get_contacts() const {
    return m_contacts;
}

/* Private */

bool Contacts::handle_command(int t_socket_fd, std::string& t_cmd, std::string& t_reply){

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