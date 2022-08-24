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
        {Job::LOGIN,            [this](Job &t_job){slots_login(t_job);}},
        {Job::DISP_CONTACTS,    [this](Job &t_job){slots_disp_contact(t_job);}},
        {Job::GETUSER,          [this](Job &t_job){slots_setuser(t_job);}},
        {Job::CREATE,           [this](Job &t_job){slots_create(t_job);}}
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

void Program::slots_login(Job &t_job)
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

void Program::slots_disp_contact(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        return;
    }

    m_chat->load_contacts(t_job.m_vector);
}

void Program::slots_setuser(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        return;
    }

    m_chat->set_user(QString::fromUtf8(t_job.m_string.c_str()));
}

void Program::slots_create(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Registeration", "Couldn't register account!");
        return;
    }

    QMessageBox::information(nullptr, "register", "Account creating was success!");
    m_welcome->hide_group("register");
}
