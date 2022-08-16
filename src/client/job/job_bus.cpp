#include "job_bus.hpp"
#include "job_queue.hpp"
#include "client.hpp"
#include "logger.hpp"

#include <vector>
#include <ctime>
#include <iostream>

bool JobBus::m_exit_loop = false;
JobBus::b_JobsMap JobBus::m_JobBus_map_bool = {};
JobBus::s_JobsMap JobBus::m_JobBus_map_string = {};
JobBus::v_JobsMap JobBus::m_JobBus_map_vector = {};

JobBus::JobBus() {
    
    m_JobBus_map_bool.emplace(Job::LIST,                               Client::contact_list);
    m_JobBus_map_bool.emplace(Job::SEARCH,                             Client::contact_search);
    m_JobBus_map_bool.emplace(Job::ADD,                                Client::contact_add_user);
    m_JobBus_map_bool.emplace(Job::REMOVE,                             Client::contact_remove_user);
    m_JobBus_map_bool.emplace(Job::AVAILABLE,                          Client::contact_available);
    m_JobBus_map_bool.emplace(Job::SETUSER,                            Client::user_set_username);
    m_JobBus_map_bool.emplace(Job::SETPASS,                            Client::user_set_password);
    m_JobBus_map_bool.emplace(Job::CREATE,                             Client::user_register_user);
    m_JobBus_map_bool.emplace(Job::LOGIN,                              Client::user_login);
    m_JobBus_map_bool.emplace(Job::LOGGED,                             Client::user_get_logged_in);
    
    m_JobBus_map_string.emplace(Job::GETUSER,                          Client::user_get_username);
    
    m_JobBus_map_vector.emplace(Job::DISP_CONTACTS,                    Client::contact_get_contacts);
}

void JobBus::main_loop() {

    time_t now;
    time(&now);

    Job job;
    while( false == m_exit_loop ) {

        if ( false == jobQ.empty()){
            jobQ.pop_try(job);

            if (job.command <= 19){
                m_JobBus_map_bool[job.command](job.argument, job.bool_value);
                jobQ.push_res(job);
            }else if (job.command <= 29) {
                m_JobBus_map_string[job.command](job.argument, job.s_value);
                 if(true == job.s_value.empty()){
                    job.bool_value = true;
                }
                jobQ.push_res(job);
            }else if (job.command <= 30) {
                m_JobBus_map_vector[job.command](job.argument, job.v_value);
                if(true == job.v_value.empty()){
                    job.bool_value = true;
                }
                jobQ.push_res(job);
            }
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
