#ifndef JOB_BUS_H
#define JOB_BUS_H

//#include "client.hpp"
#include "thread_safe_queue.hpp"
#include "job.hpp"

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
    static void handle();
    static void create(Job &t_job);
    static void set_exit();
    static void create(Job &&t_job);
    static void create_response(Job && t_job);
    static bool get_response(Job &t_job);
    static JobBus* get_instance();
    static void repeated_tasks();
    static void timer_start();

signals:
    void job_ready();
    void new_job();

private:
    static bool                  m_exit_loop;
    static JobsMap               m_JobBus_map;
    static ThreadSafeQueue<Job>  m_jobQ;
    static ThreadSafeQueue<Job>  m_resQ;
    static JobBus               *m_instance;
};


#endif // JBUS_H
