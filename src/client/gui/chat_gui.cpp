#include "chat_gui.hpp"
#include "ui/ui_chat.h"
#include "logger.hpp"
#include "fail_if.hpp"
#include "chat.hpp"

#include <QStringListModel>
#include <QVector>
#include <QList>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QStyle>
#include <QDebug>
#include <QStandardItemModel>
#include <qicon.h>
#include <qstandarditemmodel.h>

bool first_display = true;
QString TODAY = "";
QString YESTERDAY = "";

ChatGui::ChatGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChatGui)
{
    m_ui->setupUi(this);
    // Set the window to open the center of the screen with a fixed size
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),qApp->desktop()->availableGeometry()));
    this->setFixedSize(QSize(892, 700));

    QObject::connect(this, &ChatGui::on_send_clicked,                this, &ChatGui::send_msg);
    QObject::connect(this, &ChatGui::on_message_txt_returnPressed,   this, &ChatGui::send_msg);

    m_ui->search->setAutoDefault(false);
    m_ui->add->setAutoDefault(false);
    m_ui->remove->setAutoDefault(false);
}


ChatGui::~ChatGui()
{
    emit wrapping();
    delete m_ui;
}

// ***** PUBLIC ***** //

void ChatGui::init()
{
    m_notification = new Notification();
    m_ui->chat_group->hide();
    JobBus::create({Job::GETUSER});
    JobBus::create({Job::LIST});   
    TODAY =  QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd"));
    YESTERDAY =  QDateTime::currentDateTime().addDays(-1).toString(QLatin1String("yyyy-MM-dd"));
}

void ChatGui::job_set_user(Job &t_job)
{
    if ( false == t_job.m_valid)
    {
        return;
    }

    m_user = QString::fromStdString(t_job.m_string);
    m_ui->username->setText("Welcome " + m_user);
}

void ChatGui::job_disp_contact(Job &t_job)
{
    LOG_INFO("Displaying contacts");
    if ( false == t_job.m_valid)
    {
        return;
    }

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

void ChatGui::job_add_user(Job &t_job)
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

void ChatGui::job_search(Job &t_job)
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

void ChatGui::job_remove_user(Job &t_job)
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

void ChatGui::reject()
{
  QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Closing MySkype", "Are you sure you want to close MySkype?",
                                                             QMessageBox::Ok | QMessageBox::Cancel);


  if ( QMessageBox::Ok == ret )
    {
      QDialog::reject();
    }
}
           
void ChatGui::job_load_chat(Job &t_job)
{

     if ( false == t_job.m_valid)
    {
        return;
    }

    if ( true == t_job.m_chats.empty() || true == m_contact_list.empty())
    {
        return;
    }

    LOG_INFO("Loading Chat first time...");
    load_chat(t_job.m_chats, false);
    LOG_INFO("Loaded Chat first time");
    emit ready_signal();
}

void ChatGui::job_load_pending(Job &t_job)
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

void ChatGui::job_set_id(Job &t_job)
{
    m_user_id = t_job.m_intValue;
}

void ChatGui::job_send_msg(Job &t_job)
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

// ***** PRIVATE ***** //

void ChatGui::send_msg()
{
    if(m_ui->message_txt->text() == ""){
        return;
    }

    QString time = QDateTime::currentDateTime().toString(QLatin1String("hh:mm"));
    QString user = m_current_selected.data(Qt::DisplayRole).toString();
    auto contact = search_contact_list(user, "username");

    if ( 0 == contact)
    {
        return;
    }
    
    Job job;
    job.m_command = Job::SEND;
    job.m_qstring = time;
    job.m_intValue = contact;
    job.m_string = m_ui->message_txt->text().toStdString();
    job.m_argument = std::to_string(job.m_intValue) + " " + job.m_string;
    JobBus::create(job);

    m_ui->message_txt->setText("");
}

void ChatGui::display_chat(QString &t_contact)
{
    auto contact = search_contact_list(t_contact, "username");

    if (contact == 0)
    {
        return;
    }
  
    m_ui->chat_box->setModel(new QStringListModel(m_contact_chat[contact]));
    m_ui->chat_box->scrollToBottom();
}

void ChatGui::load_chat(QVector<Chat> &chats, bool t_notification)
{
    for (auto &chat : chats)
    {   
        std::string time;
        if ( TODAY == QString::fromStdString(chat.created_at_date()))
        {
            time = "Today " + chat.created_at_time();
        } else if ( YESTERDAY == QString::fromStdString(chat.created_at_date()))
        {
            time = "Yesterday " + chat.created_at_time();
        }else
        {
            time = chat.created_at_date() + " " + chat.created_at_time();
        }

        if ( chat.sender() == m_user_id)
        {
            m_contact_chat[chat.recipient()].append(m_user);
            m_contact_chat[chat.recipient()].append(QString::fromStdString(time) + "\n" + QString::fromStdString(chat.text()+ "\n"));           
        }
        else
        {  
            m_contact_chat[chat.sender()].append(m_contact_list[chat.sender()].username);
            m_contact_chat[chat.sender()].append(QString::fromStdString(time) + "\n" + QString::fromStdString(chat.text()+ "\n"));   
             if ( true == t_notification)
            {         
                m_notification->setPopupText((m_contact_list[chat.sender()].username), (QString::fromStdString(chat.text().substr(0,20))));
                m_notification->show();       
            }        
        }

        if ( false == chat.delivered())
        {
            JobBus::create({Job::DELIVERED, std::to_string(chat.id())});
        } 
    }
}

template <typename T>
int ChatGui::search_contact_list(T t_value, QString t_type)
{
    
    for (auto contact = m_contact_list.begin(); contact != m_contact_list.end(); contact++)
    {
        if (t_type == "online" && t_value == contact->online)
        {
            return contact->ID;
        }

        if (t_type == "username" && t_value == contact->username)
        {
            return contact->ID;
        } 
    }

    return 0;
}
// ***** SLOTS ***** //

void ChatGui::on_contact_list_clicked(const QModelIndex &index)
{
    QString item = index.data(Qt::DisplayRole).toString();

    if (index == m_current_selected){
        return;
    }

    m_current_selected = index;

    m_ui->chat_group->show();
    m_ui->contact_txt->setText(item);
    display_chat(item);
}

void ChatGui::on_search_clicked()
{
    m_contact.set_option("Search");
    m_contact.show();
    m_contact.raise();
}

void ChatGui::on_add_clicked()
{
    m_contact.set_option("Add");
    m_contact.show();
    m_contact.raise();
}

void ChatGui::on_remove_clicked()
{
    QString q_user =  m_current_selected.data().toString();
    std::string user = q_user.toStdString();

    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Remove " + q_user, "Are you sure you want to remove " + q_user + "?",
                                                               QMessageBox::Cancel | QMessageBox::Ok);
    if ( QMessageBox::Cancel == ret )
    {
        return;
    }

    JobBus::create({Job::REMOVE, user});
}
