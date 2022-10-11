#include "call_gui.hpp"
#include "job_bus.hpp"
#include "job.hpp"
#include "ui/ui_call.h"
#include "logger.hpp"
#include "peer_to_peer.hpp"

#include <iostream>

#include <string>
#include <QMessageBox>

#include <QCamera>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QVideoSink>


CallGui::CallGui(QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::CallGui)
{
  m_ui->setupUi(this);
}

CallGui::~CallGui()
{
  delete m_ui;
}

// ***** PUBLIC ***** //

void CallGui::call_init(int t_contact_id, QString &t_username)
{
  this->setWindowTitle("Call with " + t_username);
  m_ui->webcam->setChecked(true);
  this->show();

  Job job = {Job::CONNECT};
  job.m_intValue = t_contact_id;
  job.m_valid = false;
  JobBus::create(job);
  
}


void CallGui::video_init(int t_contact_id, QString &t_username)
{
  this->setWindowTitle("Call with " + t_username);
  this->show();
  Job job = {Job::CONNECT};
  job.m_intValue = t_contact_id;
  job.m_valid = false;

  QVideoWidget *videoWidget = new QVideoWidget(m_ui->frame);
  videoWidget->resize(m_ui->frame->width(),m_ui->frame->height());
  videoWidget->show();
  
  QCamera *camera = new QCamera(QMediaDevices::defaultVideoInput());

  camera->start();


  QMediaCaptureSession *session = new QMediaCaptureSession;
  session->setCamera(camera);
  session->setVideoOutput(videoWidget);

  QVideoSink * sink = new QVideoSink(session);

  QSize s = sink->videoSize();
  qInfo() << s;

  JobBus::create(job);
  JobBus::create({Job::WEBCAM});
}

// ***** PRIVATE ***** //

void CallGui::reject()
{
   QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Closing call", "Are you sure you want to close this call?",
                                                               QMessageBox::Ok | QMessageBox::Cancel);


   if ( QMessageBox::Ok == ret )
   {
        JobBus::create({Job::HANGUP});
        this->hide();
   }
}

// ***** SLOTS ***** //

void CallGui::on_webcam_clicked()
{
    JobBus::create({Job::WEBCAM});
}


void CallGui::on_microphone_clicked()
{
    JobBus::create({Job::MUTE});
}


void CallGui::on_hangup_clicked()
{
    JobBus::create({Job::HANGUP});
    this->hide();
}


