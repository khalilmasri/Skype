#include "call_gui.hpp"
#include "job_bus.hpp"
#include "job.hpp"
#include "ui/ui_call.h"
#include "logger.hpp"
#include "peer_to_peer.hpp"
#include <string>

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

void CallGui::call_init(std::string &t_token, int t_contact_id, QString &t_username)
{
  this->setWindowTitle("Call with " + t_username);
  m_ui->webcam->setChecked(true);
  this->show();

  Job job = {Job::CONNECT};
  job.m_intValue = t_contact_id;
  job.m_valid = false;
  JobBus::create(job);
  
}


void CallGui::video_init(std::string &t_token, int t_contact_id, QString &t_username)
{
  this->setWindowTitle("Call with " + t_username);
  this->show();
  Job job = {Job::CONNECT};
  job.m_intValue = t_contact_id;
  job.m_valid = false;
  JobBus::create(job);
}

// ***** PRIVATE ***** //

bool CallGui::connect_to(P2P &t_call, std::string t_id) {

  t_call.connect_peer(t_id);

  if (t_call.status() != P2P::Awaiting) {
    LOG_ERR("did not call connect correctly. Exiting...");
    return false;
  }

  int count = 0;

  while (t_call.status() != P2P::Accepted) {

    std::cout << "Pinging ...\n";

    t_call.ping_peer();
    sleep(1); // check every  2 second

    if (t_call.status() == P2P::Awaiting) {
      LOG_DEBUG("Still Awaiting...");
    }
    
    if (t_call.status() == P2P::Error) {
      LOG_ERR("ping returned an error. exiting....");
      return false;
    }

    if (count > 20) {
      LOG_INFO("Breaking after %d seconds", count);
      t_call.hangup_peer();
      return false;
    }

    count++;
  }

  LOG_INFO("Call accepted");
  return true;
}

// ***** SLOTS ***** //
