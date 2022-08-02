#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H

#include <iostream>
#include <string>
#include <map>

#define MAX_MSG_LEN 1024 

class Contacts {

public:
    bool list(int t_socket_fd);
    bool search(int t_socket_fd, std::string& t_cmd);
    bool add_user(int t_socket_fd, std::string& t_cmd);
    bool remove_user(int t_socket_fd, std::string& t_cmd);
    bool available(int t_socket_fd, std::string& t_cmd);

    std::vector<std::string> get_contacts() const;

private:
    std::map<std::string /* Name */, bool /* Connected */> m_contacts;

};

#endif