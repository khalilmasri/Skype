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

bool first_refresh = true;
bool first_display = true;
QTimer *timer = new QTimer();

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
    for ( auto &chat : m_contact_chat)
    {
        delete chat;
    }
    delete m_ui;
    delete timer;
}

// ***** PUBLIC ***** //

void ChatGui::init()
{
    m_ui->chat_group->hide();
    refresh_contacts();
    JobBus::handle({Job::GETUSER});
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
    if ( false == t_job.m_valid)
    {
        return;
    }


    for ( auto &contact : t_job.m_contact_list)
    {
        m_ui->contact_list->addItem(contact);
    }

    m_contact_list = t_job.m_contact_list;

    if ( true == first_display)
    {
        setup_contact_chat(FIRST);
        JobBus::handle({Job::CHAT});
        first_display = false;
    }

    if (m_current_selected.isValid() == true)
    {
        m_ui->contact_list->selectionModel()->select(m_current_selected,  QItemSelectionModel::Select);
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
    setup_contact_chat(ADD, user);
}

void ChatGui::job_search(Job &t_job)
{
    QString user = QString::fromStdString(t_job.m_argument);

    if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Search " + user, "Couldn't find " + user + " in the database!");
        return;
    }

    QMessageBox::StandardButton ret = QMessageBox::information(nullptr, "Search " + user,
                                                               user + " was found, would you like to add him?",
                                                               QMessageBox::Cancel | QMessageBox::Ok);

    if ( QMessageBox::Cancel == ret)
    {
        return;
    }

    JobBus::handle({Job::ADD, t_job.m_argument});
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
    setup_contact_chat(REMOVE, user);
}

void ChatGui::reject()
{
  QMessageBox::StandardButton ret = QMessageBox::information(nullptr, "Closing MySkype", "Are you sure you want to close MySkype?",
                                                             QMessageBox::Ok | QMessageBox::Cancel);


  if ( QMessageBox::Ok == ret )
    {
      QDialog::reject();
    }
}

void ChatGui::job_load_chat(Job &t_job)
{

    if ( true == t_job.m_chats.empty() || true == m_contact_list.empty())
    {
        return;
    }

    for (auto &chat : t_job.m_chats)
    {
        QStringList data;
        data = m_contact_chat[chat.id()]->stringList();
        data.append(QString::fromStdString(chat.created_at()));
        data.append(QString::fromStdString(chat.text()));
        m_contact_chat[chat.id()]->setStringList(data);
    }
}

//void ChatGui::job_send_msg(Job &t_job)
//{
//    if ( false == t_job.m_valid)
//    {
//        return;
//    }
//
//    QString user = m_current_selected.data(Qt::DisplayRole).toString();
//    auto contact = m_contact_list.key(user);
//
//    if ( 0 == contact)
//    {
//        return;
//    }
//
//    auto chat = m_contact_chat.find(m_contact_chat[contact].id());
//
//    if ( chat == m_contact_chat.end())
//    {
//        return;
//    }
//
//}



// ***** PRIVATE ***** //

void ChatGui::refresh_contacts()
{
    if (first_refresh == true ){
        connect(timer, &QTimer::timeout, this, &ChatGui::refresh_contacts);
        timer->start(2000);
        first_refresh = false;
    }

    JobBus::handle({Job::LIST});
}

void ChatGui::send_msg()
{
    if(m_ui->message_txt->text() == ""){
        return;
    }

    QString time = QDateTime::currentDateTime().toString(QLatin1String("hh:mm"));

    Job job;
    job.m_command = Job::SEND;
    job.m_time = time;
    JobBus::handle(job);

    m_ui->message_txt->setText("");
}

void ChatGui::display_chat(QString &t_user)
{

    auto contact = m_contact_list.key(t_user);


    if (contact == 0)
    {
        return;
    }

    m_ui->chat_box->setModel(m_contact_chat[contact]);
}

void ChatGui::setup_contact_chat(Setup t_type, const QString &t_contact)
{
    if (FIRST == t_type)
    {
        for (auto it = m_contact_list.begin(); it != m_contact_list.end(); it++)
        {
            m_contact_chat[it.key()] = new QStringListModel;
        }
    }

    auto contact = m_contact_list.key(t_contact);
    if (contact == 0)
    {
        return;
    }

    if (ADD == t_type)
    {
        m_contact_chat[contact] = new QStringListModel;
    }
    else if (REMOVE == t_type)
    {
        delete m_contact_chat[contact];
    }
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

    QMessageBox::StandardButton ret = QMessageBox::information(nullptr, "Remove " + q_user, "Are you sure you want to remove " + q_user + "?",
                                                               QMessageBox::Abort | QMessageBox::Ok);
    if ( QMessageBox::Abort == ret )
    {
        return;
    }

    JobBus::handle({Job::REMOVE, user});
}
