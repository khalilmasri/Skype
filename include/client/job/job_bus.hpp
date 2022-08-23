#ifndef JOB_BUS_H
#define JOB_BUS_H

#include "client.hpp"
#include "job_queue.hpp"

#include <unordered_map>
#include <functional>
#include <queue>
#include <QObject>
#include <QThread>

class JobBus : public QObject
{
    typedef Job::Type Type;
    typedef std::function<void (Job &t_job)> JobsMethod;
    typedef std::unordered_map<Type, JobsMethod> JobsMap;

    Q_OBJECT
public:

    JobBus(){};
    ~JobBus();
    
    // Job functionality 
    static void main_loop();
    static void set_exit();
    static void handle(Job &t_job);
    static void handle(Job &&t_job);
    static bool get_response(Job &t_job);
    static JobBus* get_instance();

signals:
    void job_ready();

private:
    static bool             m_exit_loop;
    static JobsMap          m_JobBus_map;
    static JobQueue         m_jobQ;
    static JobQueue         m_resQ;
    static JobBus           *m_instance;
};


#endif // JBUS_H
