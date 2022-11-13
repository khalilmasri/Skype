#include "central_gui.hpp"
#include "contacts.hpp"
#include "logger.hpp"
#include "ui/ui_central.h"
#include "ring_gui.hpp"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QHash>

bool first_display = true;

QHash<int , RingGui*> callers_table;

void CentralGui::job_set_user(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        return;
    }

    m_user = QString::fromStdString(t_job.m_string);
    m_ui->username->setText("Welcome " + m_user);
}

void CentralGui::job_disp_contact(Job &t_job)
{
    LOG_INFO("Displaying contacts");
    // if ( false == t_job.m_valid)
    // {
    //     return;
    // }

    auto model = new QStandardItemModel(this);
    m_ui->contact_list->setModel(model);

    for ( auto &contact : t_job.m_contact_list)
    {
        m_contact_list[contact.ID] = contact;
        
        if ( true == contact.online)
        {
            model->appendRow(new QStandardItem(QIcon("../misc/icons/online.png"), contact.username));
        }     
    }   

    for ( auto &contact : t_job.m_contact_list)
    {        
        if ( false == contact.online)
        {
            model->appendRow(new QStandardItem(QIcon("../misc/icons/offline.png"), contact.username));
        }     
    }

    if ( true == first_display)
    {
        LOG_INFO("Sending Chat job to bus");
        JobBus::create({Job::CHAT});
        first_display = false;
    }
   
    if (m_current_selected.isValid() == true)
    {
        m_ui->contact_list->selectionModel()->select(m_current_selected, QItemSelectionModel::Select);
    }
}

void CentralGui::job_add_user(Job &t_job)
{
    QString user = QString::fromStdString(t_job.m_argument);

    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Add " + user, "Couldn't add " + user);
        return;
    }

    QMessageBox::information(nullptr, "Add " + user, "Added " + user + " successfully!");
    m_contact.hide();
    
    // TODO: Change the behaviour later
    m_ui->contact_list->clearSelection();
    m_current_selected = m_ui->contact_list->currentIndex();
    m_ui->chat_group->hide();
}

void CentralGui::job_search(Job &t_job)
{
    QString user = QString::fromStdString(t_job.m_argument);

    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Search " + user, "Couldn't find " + user + " in the database!");
        return;
    }

    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Search " + user,
                                                               user + " was found, would you like to add him?",
                                                               QMessageBox::Cancel | QMessageBox::Ok);

    if ( QMessageBox::Cancel == ret)
    {
        return;
    }

    JobBus::create({Job::ADD, t_job.m_argument});
}

void CentralGui::job_remove_user(Job &t_job)
{
    QString user = QString::fromStdString(t_job.m_argument);

    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Remove " + user, "Removing " + user + " failed!");
        return;
    }

    QMessageBox::information(nullptr, "Remove " + user, "Removing " + user + " was successful!");

    m_ui->contact_list->clearSelection();
    m_current_selected = m_ui->contact_list->currentIndex();
    m_ui->chat_group->hide();
}

void CentralGui::job_load_chat(Job &t_job)
{

    // if ( false == t_job.m_valid)
    // {
    //     return;
    // } 

    // if ( true == t_job.m_chats.empty() || true == m_contact_list.empty())
    if (true == m_contact_list.empty())
    {
        // new logic here necessary?
        LOG_INFO("Chats and Contacts currently empty for user")
        emit ready_signal();
        return;
        // neew logic here necessary?
    }

    LOG_INFO("Loading Chat first time...");
    load_chat(t_job.m_chats, false);
    LOG_INFO("Loaded Chat first time");
    emit ready_signal();
}

void CentralGui::job_load_pending(Job &t_job)
{
     if ( false == t_job.m_valid)
    {
        return;
    }

    if ( true == t_job.m_chats.empty() || true == m_contact_list.empty())
    {
        return;
    }

    load_chat(t_job.m_chats, true);

    QString user = m_current_selected.data(Qt::DisplayRole).toString();
    display_chat(user);
}

void CentralGui::job_set_id(Job &t_job)
{
    m_user_id = t_job.m_intValue;
}

void CentralGui::job_send_msg(Job &t_job)
{   

   if ( false == t_job.m_valid)
   {
       return;
   }

    QString user = m_current_selected.data(Qt::DisplayRole).toString();
    auto contact = search_contact_list(user, "username");

    if ( 0 == contact)
    {
        return;
    }
    
    m_contact_chat[t_job.m_intValue].append(m_user);
    m_contact_chat[t_job.m_intValue].append("Today " + t_job.m_qstring);
    m_contact_chat[t_job.m_intValue].append(QString::fromStdString(t_job.m_string + "\n"));
    
    display_chat(user);
}

void CentralGui::job_hangup(Job &t_job)
{
    static_cast<void>(t_job); // NOTE(@khalil) What is this static cast to nothing?
    LOG_INFO("Hangup call");
    m_on_call = false;

    delete m_call;
}

void CentralGui::job_awaiting(Job &t_job)
{
  
  if ( false == t_job.m_boolValue)
  {
    callers_table[t_job.m_intValue]->hide();
    delete callers_table[t_job.m_intValue];
    callers_table.remove(t_job.m_intValue);
    return;
  }

  LOG_INFO("Incoming call!");
  
  RingGui *ring = new RingGui(this);
  QObject::connect(ring, &RingGui::start_call, this, &CentralGui::started_call);
  QString username = m_contact_list[t_job.m_intValue].username;
  ring->set_details(username, t_job.m_intValue );
  callers_table.insert(t_job.m_intValue, ring);
  ring->show(); 
}


void CentralGui::job_video_stream(Job &t_job){

   if(t_job.m_video_stream == nullptr){
     LOG_ERR("Cannot find video stream in Job::VIDEO_STREAM. Quiting showing video...");
     return;
   }

  auto queue = t_job.m_video_stream;

  // ---> coming jpeg.

   Webcam::WebcamMat mat; // ---> open cv::Mat (not QT);
   bool valid = queue->pop_try(mat);

   if(valid){
     // do something with the Mat.
     Webcam::wait(); // wait to show next frame if needed
   }

}
