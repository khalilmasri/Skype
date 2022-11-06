#include "ring_gui.hpp"
#include "central_gui.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "ui/ui_ring.h"

#include <QMovie>
#include <qmovie.h>

RingGui::RingGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::RingGui)
{
    m_ui->setupUi(this);

    m_ui->answer->setAutoDefault(false);
    m_ui->ignore->setAutoDefault(false);
}

RingGui::~RingGui()
{
    delete m_ui;
}
void RingGui::set_details(QString &t_user, int t_id)
{
    this->setWindowTitle("Incoming call from " + t_user);
    m_ui->details->setText("Would you like to accept the call from " + t_user + "?");
    m_caller_id = t_id;
}

/***** SIGNALS ******/ 

void RingGui::on_answer_clicked()
{
    Job job = {Job::ACCEPT};
    job.m_intValue = m_caller_id;
    JobBus::create(job);
    LOG_INFO("Accepted call");

    emit start_call(m_caller_id);
  
    this->hide();
}

void RingGui::on_ignore_clicked()
{
    Job job = {Job::REJECT};
    job.m_intValue = m_caller_id;
    JobBus::create(job);
    
    this->hide();
}

