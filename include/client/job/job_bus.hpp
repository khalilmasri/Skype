#ifndef JOB_BUS_H
#define JOB_BUS_H

#include "client.hpp"
#include "job_queue.hpp"

#include <unordered_map>
#include <functional>
#include <queue>

class JobBus{
    typedef Job::Type Type;
    typedef std::function<void (std::string &t_arg, void* t_ret)> JobsMethod;
    typedef std::unordered_map<Type, JobsMethod> JobsMap;

public:

    JobBus();
    
    // Job functionality 
    static void main_loop();
    static void set_exit();

private:
    static bool             m_exit_loop;
    static JobsMap          m_JobBus_map; 

};


#endif // JBUS_H