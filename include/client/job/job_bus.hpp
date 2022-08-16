#ifndef JOB_BUS_H
#define JOB_BUS_H

#include "client.hpp"
#include "job_queue.hpp"

#include <unordered_map>
#include <functional>
#include <queue>

class JobBus{
    typedef Job::Type Type;
    
    // Bool jobs handling
    typedef std::function<void (std::string &t_arg, bool &t_ret)> b_JobsMethod;
    typedef std::unordered_map<Type, b_JobsMethod> b_JobsMap;

    // std::string job handling
    typedef std::function<void (std::string &t_arg, std::string &t_ret)> s_JobsMethod;
    typedef std::unordered_map<Type, s_JobsMethod> s_JobsMap;

    // std::vector<std::string> job handling
    typedef std::function<void (std::string &t_arg, std::vector<std::string> &t_ret)> v_JobsMethod;
    typedef std::unordered_map<Type, v_JobsMethod> v_JobsMap;
public:

    JobBus();
    
    // Job functionality 
    static void main_loop();
    static void set_exit();

private:
    static bool             m_exit_loop;
    static b_JobsMap        m_JobBus_map_bool; 
    static s_JobsMap        m_JobBus_map_string; 
    static v_JobsMap        m_JobBus_map_vector; 

};


#endif // JBUS_H