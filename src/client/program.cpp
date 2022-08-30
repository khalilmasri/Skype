#include "program.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "fail_if.hpp"

#include <QObject>
#include <QMessageBox>
#include <QString>
#include <string>
#include <thread>
#include <iostream>

Program::Program()
{
    // Create the job dispatch table
    create_job_dispatcher();

    // Creating an instance || The reason I'm doing as singelton pattern is because we can't emit signals in static methods
    m_bus = JobBus::get_instance();

    // Connecting Signals and slots
    QObject::connect(m_bus, &JobBus::job_ready, this, &Program::handle_response);
    
    // Creating the GUI
    m_welcome = new WelcomeGui();
    m_chat = new ChatGui();

    m_welcome->show();
}

Program::~Program()
{
    delete m_welcome;
    delete m_chat;
}

// ***** PRIVATE ***** //

void Program::create_job_dispatcher()
{
    m_table = {
        {Job::LOGIN,            [this](Job &t_job){job_login(t_job);}},
        {Job::CREATE,           [this](Job &t_job){m_welcome->job_create(t_job);}},
        {Job::DISP_CONTACTS,    [this](Job &t_job){m_chat->job_disp_contact(t_job);}},
        {Job::GETUSER,          [this](Job &t_job){m_chat->job_set_user(t_job);}},
        {Job::ADD,              [this](Job &t_job){m_chat->job_add_user(t_job);}},
        {Job::REMOVE,           [this](Job &t_job){m_chat->job_remove_user(t_job);}},
        {Job::SEARCH,           [this](Job &t_job){m_chat->job_search(t_job);}},
        {Job::GETID,            [this](Job &t_job){m_chat->job_set_id(t_job);}},
        {Job::CHAT,             [this](Job &t_job){m_chat->job_load_chat(t_job);}},
        {Job::PENDING,          [this](Job &t_job){m_chat->job_load_chat(t_job);}},
        {Job::SEND,             [this](Job &t_job ){m_chat->job_send_msg(t_job);}}
    };
}

void Program::handle_response()
{
    Job job;

    FAIL_IF_SILENT( false == JobBus::get_response(job));

    try{
        m_table[job.m_command](job);
    }catch(...){} // Used to not crash the program

fail:
    return;
}

// ***** SLOTS ***** //

void Program::job_login(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Login" , "Wrong username or password!");
        return;
    }

    JobBus::create({Job::GETID});
    m_welcome->hide();
    m_chat->show();
    m_chat->init();
}
