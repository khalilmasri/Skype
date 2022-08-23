#include "program.hpp"
#include "job_bus.hpp"

#include <iostream>
#include <QObject>
#include <QtConcurrent>

Program::Program()
{
    m_bus = JobBus::get_instance();

    connect(this, &Program::wrapping, m_bus, &JobBus::set_exit);
    connect(m_bus, &JobBus::job_ready, this, &Program::handle_response);

    m_bus_loop = QThread::create(&JobBus::main_loop);
    m_bus_loop->start();

    m_welcome = new WelcomeGui();
    m_chat = new ChatGui();

    m_welcome->show();
}

void Program::handle_response()
{
    Job job;

    JobBus::get_response(job);

    std::cout << "Job => " << job.m_command << std::endl;
}

Program::~Program()
{
    emit wrapping(); // sending a signal to close the bus
    m_bus_loop->wait();
    delete m_welcome;
    delete m_chat;
}
