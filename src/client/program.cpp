#include "program.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "fail_if.hpp"

#include <QObject>
#include <QMessageBox>
#include <QString>

#include <iostream>

Program::Program()
{
    // Create the job dispatch table
    create_job_dispatcher();

    // Creating an instance || The reason I'm doing as singelton pattern is because we can't emit signals in static methods
    m_bus = JobBus::get_instance();

    // Connecting Signals and slots
    connect(this, &Program::wrapping, m_bus, &JobBus::set_exit);
    connect(m_bus, &JobBus::job_ready, this, &Program::handle_response);

    // Creating the main thread loop
    m_bus_loop = QThread::create(&JobBus::main_loop);
    m_bus_loop->start();

    // Creating the GUI
    m_welcome = new WelcomeGui();
    m_chat = new ChatGui();

    m_welcome->show();
}

Program::~Program()
{
    emit wrapping(); // sending a signal to close the bus
    m_bus_loop->wait();

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
        {Job::SEARCH,           [this](Job &t_job){m_chat->job_search(t_job);}}
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

    m_welcome->hide();
    m_chat->show();
    m_chat->init();
}