#include "call_gui.hpp"
#include "call.hpp"
#include "job_bus.hpp"
#include "job.hpp"
#include "ui/ui_call.h"
#include "logger.hpp"
#include "peer_to_peer.hpp"
#include "menu_gui.hpp"
#include "video_playback.hpp"

#include <iostream>

#include <string>
#include <QMessageBox>

#include <QCamera>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QVideoSink>
#include <QPixmap>


CallGui::CallGui(QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::CallGui) {
  m_ui->setupUi(this);
}

CallGui::~CallGui() {
  delete m_ui;
}

// ***** PUBLIC ***** //

void CallGui::call_accept(QString &t_username)
{
  this->setWindowTitle("Call with " + t_username);
  m_ui->webcam->setChecked(true);

  this->show();

}

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
  JobBus::create(job);

  JobBus::create({Job::WEBCAM});
}

void CallGui::video_stream(VideoPlayback::VideoQueuePtr t_stream_queue)
{
  
   QThread *video_stream = QThread::create([t_stream_queue, this](){
    cv::Mat frame;
    size_t trials = 0;
    while(!m_stop_stream)
    {
      if(trials > VideoPlayback::m_MID_PLAYBACK_THROTTLE){
       LOG_TRACE("Throttling video playback to 50ms");
         std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }

      if(trials > VideoPlayback::m_MAX_PLAYBACK_THROTTLE){
       LOG_TRACE("Throttling video playback to 100ms");
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
     }

      if (t_stream_queue->empty()) {
        trials++;
        continue;
      }

      bool valid = t_stream_queue->pop_try(frame);
      if (valid)
      {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
        QImage frame_draw(static_cast<const unsigned char*>(frame.data), frame.cols * 3, frame.rows * 3, QImage::Format_RGB888);
        m_ui->camera->setPixmap(QPixmap::fromImage(frame_draw));
        m_ui->camera->resize(m_ui->camera->pixmap().size());
        qDebug() << m_ui->camera->pixmap().size();
        trials = 0;
        Webcam::wait();
      }
    }
   });
  
  video_stream->start();

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

void CallGui::on_hangup_clicked()
{
    JobBus::create({Job::HANGUP});
    m_stop_stream = true;
    this->hide();
}

