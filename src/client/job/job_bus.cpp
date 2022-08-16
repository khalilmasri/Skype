#include "job_bus.hpp"
#include "job_queue.hpp"
#include "client.hpp"

#include <vector>
#include <ctime>

bool JobBus::m_exit_loop = false;
JobBus::JobsMap JobBus::m_JobBus_map = {};

JobBus::JobBus() {
    
    m_JobBus_map.emplace(Job::LIST,                               Client::contact_list);
    m_JobBus_map.emplace(Job::SEARCH,                             Client::contact_search);
    m_JobBus_map.emplace(Job::ADD,                                Client::contact_add_user);
    m_JobBus_map.emplace(Job::REMOVE,                             Client::contact_remove_user);
    m_JobBus_map.emplace(Job::AVAILABLE,                          Client::contact_available);
    m_JobBus_map.emplace(Job::SETUSER,                            Client::user_set_username);
    m_JobBus_map.emplace(Job::SETPASS,                            Client::user_set_password);
    m_JobBus_map.emplace(Job::CREATE,                             Client::user_register_user);
    m_JobBus_map.emplace(Job::LOGIN,                              Client::user_login);
    m_JobBus_map.emplace(Job::LOGGED,                             Client::user_get_logged_in);
    // m_JobBus_map.emplace(Job<std::string>::GETUSER,                     Client::user_get_username);
    // m_JobBus_map.emplace(Job<std::vector<std::string>>::DISP_CONTACTS,  Client::contact_get_contacts);
}

void JobBus::main_loop() {

    time_t now;
    time(&now);

    while( false == m_exit_loop ) {

        if ( 1 <= jobQ.size){
            std::cout << "Size " <<jobQ.jobs.size() << std::endl;
            Job job;
            jobQ.pop_job(job);
            std::cout << job.command << std::endl;
            m_JobBus_map[job.command](job.argument, job.return_value);
        }

        // if (difftime(time(NULL), now) > 3){ // Run this task every 3 seconds
        //     std::string me = "";
        //     contact_list(me);
        //     time(&now);
        // }
    }
}

void JobBus::set_exit(){
    m_exit_loop = true;
}
