#ifndef JOB_BUS_H
#define JOB_BUS_H

#include "client.hpp"
#include "job_queue.hpp"

#include <unordered_map>
#include <functional>
#include <queue>

class JobBus{

    typedef Job::Type Type;
    typedef std::function<bool (std::string&)> JobsMethod;
    typedef std::unordered_map<Type, JobsMethod> JobsMap;

public:

    JobBus(int t_port);
    
    // Job functionality 
    static void main_loop();
    static void set_exit();

    // Contacts 
    static bool contact_list(std::string &_);
    static bool contact_search(std::string &t_arg);
    static bool contact_add_user(std::string &t_arg);
    static bool contact_remove_user(std::string &t_arg);
    static bool contact_available(std::string &t_arg);
    static std::vector<std::string> contact_get_contacts();

    // User
    static bool user_set_username(std::string &t_arg);
    static bool user_set_password(std::string &t_arg);

    static bool user_register_user(std::string &_);
    static bool user_login(std::string &_);
    static bool user_get_logged_in(std::string &_); 

    static std::string user_get_username();

private:
    static Client           m_client;
    static bool             m_exit_loop;
    static JobsMap          m_JobBus_map; 
};


#endif // JBUS_H