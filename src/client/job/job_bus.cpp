#include "job_bus.hpp"
#include "client.hpp"
#include "job_queue.hpp"

#include <ctime>
#include <iostream>
#include <vector>
#include <QThread>

bool JobBus::m_exit_loop = false;
JobBus* JobBus::m_instance = nullptr;

JobQueue JobBus::m_jobQ;
JobQueue JobBus::m_resQ;

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
    {Job::GETUSER,          Client::user_get_username},
    {Job::GETCONT,          Client::contact_get_current_contact},
    {Job::DISP_CONTACTS,    Client::contact_get_contacts}
};

JobBus* JobBus::get_instance()
{
    if(!m_instance){
        m_instance = new JobBus();
    }

    return m_instance;
}

JobBus::~JobBus()
{
}

void JobBus::handle(Job &&t_job){
    m_jobQ.push(t_job);
}

void JobBus::handle(Job &t_job){
    m_jobQ.push(t_job);
}

void JobBus::main_loop() {
   
    Job job;

    while (false == m_exit_loop) {

        if (false == m_jobQ.empty()) {
            
            bool res = m_jobQ.pop_try(job);

            if ( false == res ) {
                continue;
            }

            m_JobBus_map[job.m_command](job);
            m_resQ.push(job);
            emit JobBus::get_instance()->job_ready();
        }
    }
  
}

bool JobBus::get_response(Job &t_job){

    if (true == m_resQ.empty()){
        return false;
    }

    bool res = m_resQ.pop_try(t_job);

    if (false == res){
        return false;
    }

    return true;
} 

void JobBus::set_exit() { m_exit_loop = true; }
