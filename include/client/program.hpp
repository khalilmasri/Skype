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
    Q_OBJECT
public:
    Program();
    ~Program();

signals:
    void wrapping();

private:
    Client      m_client;
    JobBus      *m_bus;
    WelcomeGui  *m_welcome;
    ChatGui     *m_chat;
    QThread     *m_bus_loop;

    void handle_response();
};

#endif // PROGRAM_H
