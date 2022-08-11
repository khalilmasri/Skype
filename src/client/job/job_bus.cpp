#include "job_bus.hpp"
#include "client.hpp"
#include "supress_unused_params_warnings.hpp"

#include <vector>
#include <ctime>

Client JobBus::m_client = Client(0);
bool JobBus::m_exit_loop = false;
JobBus::JobsMap JobBus::m_JobBus_map = {};

JobBus::JobBus(int t_port) {
    
    m_client = Client(t_port);

    m_JobBus_map.emplace(Job::LIST,                 &JobBus::contact_list);
    m_JobBus_map.emplace(Job::SEARCH,               &JobBus::contact_search);
    m_JobBus_map.emplace(Job::ADD,                  &JobBus::contact_add_user);
    m_JobBus_map.emplace(Job::REMOVE,               &JobBus::contact_remove_user);
    m_JobBus_map.emplace(Job::AVAILABLE,            &JobBus::contact_available);
    m_JobBus_map.emplace(Job::SETUSER,              &JobBus::user_set_username);
    m_JobBus_map.emplace(Job::SETPASS,              &JobBus::user_set_password);
    m_JobBus_map.emplace(Job::CREATE,               &JobBus::user_register_user);
    m_JobBus_map.emplace(Job::LOGIN,                &JobBus::user_login);
    m_JobBus_map.emplace(Job::LOGGED,               &JobBus::user_get_logged_in);
}

void JobBus::main_loop() {

    time_t now;
    time(&now);
    
    while( false == m_exit_loop ) {
        
        if ( false == jobQ.jobs.empty()){
            Job job;
            jobQ.pop_job(job);
            m_JobBus_map[job.command](job.argument);
        }

        if (difftime(time(NULL), now) > 3){ // Run this task every 3 seconds
            std::string me = "";
            contact_list(me);
            time(&now);
        }
    }
}

void JobBus::set_exit(){
    m_exit_loop = true;
}

bool JobBus::contact_list(std::string &_){
    UNUSED_PARAMS(_);

    return m_client.contact_list();
}

bool JobBus::contact_search(std::string &t_arg){
    return m_client.contact_search(t_arg);
}

bool JobBus::contact_add_user(std::string &t_arg){
    return m_client.contact_add_user(t_arg);
}

bool JobBus::contact_remove_user(std::string &t_arg) {
    return m_client.contact_remove_user(t_arg);
}

bool JobBus::contact_available(std::string &t_arg){
    return m_client.contact_available(t_arg);
}

std::vector<std::string> JobBus::contact_get_contacts(){
    return m_client.contact_get_contacts();
}

bool JobBus::user_set_username(std::string &t_arg) {
    return m_client.user_set_username(t_arg);
}

bool JobBus::user_set_password(std::string &t_arg){
    return m_client.user_set_password(t_arg);
}

bool JobBus::user_register_user(std::string &_){
    UNUSED_PARAMS(_);
    return m_client.user_register_user();
}

bool JobBus::user_login(std::string &_){
    UNUSED_PARAMS(_);
    return m_client.user_login();
}

bool JobBus::user_get_logged_in(std::string &_){
    UNUSED_PARAMS(_);

    return m_client.user_get_logged_in();
}

std::string JobBus::user_get_username(){
    return m_client.user_get_username();
}