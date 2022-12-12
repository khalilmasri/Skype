#include "call_gui.hpp"
#include "call.hpp"
#include "job.hpp"
#include "job_bus.hpp"
#include "logger.hpp"
#include "menu_gui.hpp"
#include "peer_to_peer.hpp"
#include "ui/ui_call.h"
#include "video_playback.hpp"

#include <iostream>

#include <QMessageBox>
#include <string>

#include <QCamera>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QPixmap>
#include <QVideoSink>
#include <QVideoWidget>

CallGui::CallGui(QWidget *parent) : QDialog(parent), m_ui(new Ui::CallGui) {
  m_ui->setupUi(this);
}

CallGui::~CallGui() { delete m_ui; }

// ***** PUBLIC ***** //

void CallGui::call_accept(QString &t_username) {
  this->setWindowTitle("Call with " + t_username);

  this->show();
}

void CallGui::call_init(int t_contact_id, QString &t_username) {
  this->setWindowTitle("Call with " + t_username);
  this->show();

  Job job = {Job::CONNECT};
  job.m_intValue = t_contact_id;
  job.m_valid = false;
  JobBus::create(job);
}

void CallGui::video_stream(VideoPlayback::VideoQueuePtr t_stream_queue) {
    QThread *video_stream = QThread::create([t_stream_queue, this]() {
    cv::Mat frame;
    size_t trials = 0;

    while (!m_stop_stream) {

      if (trials > VideoPlayback::m_MID_PLAYBACK_THROTTLE) {
        LOG_TRACE("Throttling video playback to 50ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }

      if (trials > VideoPlayback::m_MAX_PLAYBACK_THROTTLE) {
        LOG_TRACE("Throttling video playback to 100ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      if (t_stream_queue->empty()) {
        trials++;
        continue;
      }

      bool valid = t_stream_queue->pop_try(frame);

      if (valid) {
        try {
          QImage frame_draw = mat_to_qimage_ref(frame, QImage::Format_RGB888);
          cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
          m_ui->camera->setPixmap(QPixmap::fromImage(frame_draw));
          m_ui->camera->resize(m_ui->camera->pixmap().size());
          trials = 0;
          Webcam::wait();
        } catch (...) {
          LOG_ERR("CV >> QT convertion create an exception.")
        }
      }
    }
  });

  video_stream->start();
}

// ***** PRIVATE ***** //

void CallGui::reject() {
  QMessageBox::StandardButton ret = QMessageBox::question(
      nullptr, "Closing call", "Are you sure you want to close this call?",
      QMessageBox::Ok | QMessageBox::Cancel);

  if (QMessageBox::Ok == ret) {
    JobBus::create({Job::HANGUP});
    this->hide();
  }
}

QImage CallGui::mat_to_qimage_ref(cv::Mat &mat, QImage::Format format) {
  return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

// ***** SLOTS ***** //

void CallGui::on_hangup_clicked() {
  JobBus::create({Job::HANGUP});
  m_stop_stream = true;
  this->hide();
}