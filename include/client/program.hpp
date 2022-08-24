#ifndef PROGRAM_H
#define PROGRAM_H

#include "client.hpp"
#include "welcome.hpp"
#include "chat.hpp"
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

signals:
    void wrapping();

private slots:
    void handle_response();

private: // Variables
    Client      m_client;
    JobBus      *m_bus;
    WelcomeGui  *m_welcome;
    ChatGui     *m_chat;
    QThread     *m_bus_loop;
    JobDispatch m_table;

private: // Methods
    void create_job_dispatcher();

    // slots
    void slots_login(Job &t_job);
    void slots_disp_contact(Job &t_job);
    void slots_setuser(Job &t_job);
    void slots_create(Job &t_job);
};

#endif // PROGRAM_H
