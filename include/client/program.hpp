#ifndef PROGRAM_H
#define PROGRAM_H

#include "client.hpp"
#include "welcome_gui.hpp"
#include "central_gui.hpp"
#include "job_bus.hpp"
#include "job.hpp"

#include <QObject>
#include <QThread>

class Program : public QObject
{
	typedef Job::Type Type;
	typedef std::function<void (Job &t_job)> SetMethod;
	typedef std::unordered_map<Type, SetMethod> JobDispatch;

	Q_OBJECT
public:
	Program();
	~Program();

private slots:
	void handle_response();

private: // Variables
	Client      m_client;
	JobBus      *m_bus = nullptr;
	WelcomeGui  *m_welcome = nullptr;
	CentralGui     *m_central = nullptr;
	MenuGui 			*m_menu = nullptr;
	//QThread     *m_bus_loop;
	JobDispatch m_table;

private: // Methods
	void create_job_dispatcher();

	// slots
	void job_login(Job &t_job);
	void switch_to_chat();

  static const int m_MAX_TRIES = 20000;
};

#endif // PROGRAM_H
