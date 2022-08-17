#include "job_bus.hpp"
#include "client.hpp"
#include "job_queue.hpp"
#include "logger.hpp"

#include <ctime>
#include <iostream>
#include <vector>

bool JobBus::m_exit_loop = false;
time_t now;

JobBus::JobsMap JobBus::m_JobBus_map {

    {Job::LIST,             Client::contact_list},
    {Job::SELCONT,          Client::contact_set_current_contact},
    {Job::SEARCH,           Client::contact_search},
    {Job::ADD,              Client::contact_add_user},
    {Job::REMOVE,           Client::contact_remove_user},
    {Job::AVAILABLE,        Client::contact_available},
    {Job::SETUSER,          Client::user_set_username},
    {Job::SETPASS,          Client::user_set_password},
    {Job::CREATE,           Client::user_register_user},
    {Job::LOGIN,            Client::user_login}, 
    {Job::LOGGED,           Client::user_get_logged_in},
    {Job::GETUSER,          Client::user_get_username },
    {Job::GETCONT,          Client::contact_get_current_contact},
    {Job::DISP_CONTACTS,    Client::contact_get_contacts }
};

bool JobBus::handle(Job &&t_job){
    jobQ.push(t_job);
    jobQ.pop_res(t_job);

    return t_job.m_valid;
}

bool JobBus::handle(Job &t_job){
    jobQ.push(t_job);
    jobQ.pop_res(t_job);

    return t_job.m_valid;
}

void JobBus::main_loop() {
   
    time(&now);
    Job job;

    while (false == m_exit_loop) {

        if (false == jobQ.empty()) {
            
            bool res = jobQ.pop_try(job);

            if ( false == res ) {
                continue;
            }

            m_JobBus_map[job.m_command](job);
            jobQ.push_res(job);
            
        }

        repeat_job(job);    
    }
  
}

void JobBus::repeat_job(Job &t_job){
    
    m_JobBus_map[Job::LOGGED](t_job);

    if ( false == t_job.m_valid){
        return;
    }

    if (difftime(time(NULL), now) > 10){ // Run this task every 3 seconds
            std::string me = "";
            m_JobBus_map[Job::LIST](t_job);
            time(&now);
    }  
}

void JobBus::set_exit() { m_exit_loop = true; }
