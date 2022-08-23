#include "program.hpp"
#include "job_bus.hpp"
#include "logger.hpp"

#include <QObject>
#include <QDebug>

Program::Program()
{
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


void Program::handle_response()
{
    Job job;

    JobBus::get_response(job);

    if (false == job.m_valid)
    {
        return;
    }

    switch(job.m_command)
    {
        case Job::LOGIN:
            m_welcome->hide();
            m_chat->show();
            m_chat->init();
        break;

        case Job::DISP_CONTACTS:
            m_chat->load_contacts(job);
        break;

        case Job::GETUSER:
            m_chat->set_user(job);
        break;

        default:
            LOG_INFO("Event ignored -> %d", job.m_command);
        break;
    }
}

Program::~Program()
{
    emit wrapping(); // sending a signal to close the bus
    m_bus_loop->wait();

    delete m_welcome;
    delete m_chat;
}
