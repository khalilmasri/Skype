#ifndef USER_H
#define USER_H

#include <string>

#define MAX_MSG_LEN 1024

class User {

public:
    bool register_user(int t_socket_fd);
    bool login(int t_socket_fd);

    std::string get_username() const;

    bool set_username(std::string& t_username);
    bool set_password(std::string& t_password);
    
    bool get_logged_in() const;

private:
    std::string m_username;
    std::string m_password;

    bool m_logged_in = false;

    bool handle_command(int t_socket_fd, std::string& t_cmd, std::string& t_reply);
};

#endif