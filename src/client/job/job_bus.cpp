#include "job_bus.hpp"
#include "client.hpp"
#include "job_queue.hpp"
#include "logger.hpp"

#include <ctime>
#include <iostream>
#include <vector>

bool JobBus::m_exit_loop = false;

JobBus::b_JobsMap JobBus::m_JobBus_map_bool {

      {Job::LIST, Client::contact_list},
      {Job::SEARCH, Client::contact_search},
      {Job::ADD, Client::contact_add_user},
      {Job::REMOVE, Client::contact_remove_user},
      {Job::AVAILABLE, Client::contact_available},
      {Job::SETUSER, Client::user_set_username},
      {Job::SETPASS, Client::user_set_password},
      {Job::CREATE, Client::user_register_user},
      {Job::LOGIN, Client::user_login}, {
      Job::LOGGED, Client::user_get_logged_in
  }
};

JobBus::s_JobsMap JobBus::m_JobBus_map_string = {
  { Job::GETUSER, Client::user_get_username },
   {Job::GETUSER, Client::user_get_username },
};

JobBus::v_JobsMap JobBus::m_JobBus_map_vector = {

  { Job::DISP_CONTACTS, Client::contact_get_contacts }

};

void JobBus::main_loop() {

  time_t now;
  time(&now);

  Job job;
  while (false == m_exit_loop) {

    if (false == jobQ.empty()) {
      jobQ.pop_try(job);

      if (job.command <= 19) {
        m_JobBus_map_bool[job.command](job.argument, job.bool_value);
        jobQ.push_res(job);
      } else if (job.command <= 29) {
        m_JobBus_map_string[job.command](job.argument, job.s_value);
        if (false == job.s_value.empty()) {
          job.bool_value = true;
        }
        jobQ.push_res(job);
      } else if (job.command <= 30) {
        m_JobBus_map_vector[job.command](job.argument, job.v_value);
        if (false == job.v_value.empty()) {
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

void JobBus::set_exit() { m_exit_loop = true; }
