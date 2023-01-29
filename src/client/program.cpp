#include "program.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "fail_if.hpp"
#include "notification.hpp"

#include <QObject>
#include <QMessageBox>
#include <QString>
#include <string>
#include <iostream>


Program::Program()
{
	// Create the job dispatch table
	create_job_dispatcher();

	// Creating an instance || The reason I'm doing as singelton pattern is because we can't emit signals in static methods
	m_bus = JobBus::get_instance();
	
	// Creating the GUI
	m_welcome = new WelcomeGui();
	m_central = new CentralGui();

	QObject::connect(m_bus, &JobBus::job_ready, this, &Program::handle_response);
	QObject::connect(m_central, &CentralGui::ready_signal, m_welcome, &WelcomeGui::stop_loading);
	QObject::connect(m_central, &CentralGui::ready_signal, m_welcome, &JobBus::timer_start);
	QObject::connect(m_welcome, &WelcomeGui::stopped_loading, this, &Program::switch_to_chat);
	QObject::connect(m_central, &CentralGui::wrapping, m_bus, &JobBus::set_exit);

	m_welcome->show();

  int tries = 0;

	bool res = m_client.init();
	while(false == res) {
	res = m_client.init();
	usleep(300);
  tries++;

  if (tries >= m_MAX_TRIES){
    QMessageBox::warning(nullptr, "Error", "Could not connect to server.", QMessageBox::Ok);
    exit(1);
   }
	}
	
	QThread *bus_loop = QThread::create(&JobBus::handle);
	bus_loop->start();
}

Program::~Program()
{
	delete m_welcome;
	delete m_central;
	delete m_menu;
	delete m_bus;
}

// ***** PRIVATE ***** //

void Program::create_job_dispatcher()
{
	m_table = {
		{Job::LOGIN,            [this](Job &t_job){job_login(t_job);}},
		{Job::CREATE,           [this](Job &t_job){m_welcome->job_create(t_job);}},
		{Job::DISP_CONTACTS,    [this](Job &t_job){m_central->job_disp_contact(t_job);}},
		{Job::GETUSER,          [this](Job &t_job){m_central->job_set_user(t_job);}},
		{Job::ADD,              [this](Job &t_job){m_central->job_add_user(t_job);}},
		{Job::REMOVE,           [this](Job &t_job){m_central->job_remove_user(t_job);}},
		{Job::SEARCH,           [this](Job &t_job){m_central->job_search(t_job);}},
		{Job::GETID,            [this](Job &t_job){m_central->job_set_id(t_job);}},
		{Job::CHAT,             [this](Job &t_job){m_central->job_load_chat(t_job);}},
		{Job::PENDING,          [this](Job &t_job){m_central->job_load_pending(t_job);}},
		{Job::SEND,             [this](Job &t_job){m_central->job_send_msg(t_job);}},
		{Job::HANGUP,           [this](Job &t_job){m_central->job_hangup(t_job);}},
		{Job::AWAITING,         [this](Job &t_job){m_central->job_awaiting(t_job);}},
		{Job::VIDEO_STREAM,     [this](Job &t_job){m_central->job_video_stream(t_job);}},
		{Job::VIDEO_FAILED,     [this](Job &t_job){m_central->job_video_failed(t_job);}},
		{Job::AUDIO_FAILED,     [this](Job &t_job){m_central->job_audio_failed(t_job);}},
		{Job::PEER_HANGUP,     	[this](Job &t_job){m_central->job_peer_hangup(t_job);}},
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

	m_welcome->load_screen();
	JobBus::create({Job::GETID}); 
	m_central->init();
}

void Program::switch_to_chat()
{
	m_welcome->hide();
	
	QMessageBox::information(nullptr, "Input/output settings", "Please configure your audio settings!", QMessageBox::Ok);
	
	m_menu = new MenuGui();
	
	m_central->show();
	m_menu->show();
}
