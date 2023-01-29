#include "central_gui.hpp"
#include "ui/ui_central.h"
#include "logger.hpp"
#include "fail_if.hpp"
#include "chat.hpp"
#include "call_gui.hpp"
#include "ring_gui.hpp"

#include <QStringListModel>
#include <QVector>
#include <QList>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QScreen>
#include <QWindow>
#include <QMessageBox>
#include <QStyle>
#include <QDebug>
#include <QStandardItemModel>
#include <qicon.h>
#include <qnamespace.h>
#include <qstandarditemmodel.h>
#include <QAudioOutput>

QString TODAY = "";
QString YESTERDAY = "";

CentralGui::CentralGui(QWidget *parent) : QDialog(parent),
                                          m_ui(new Ui::CentralGui)
{
    m_ui->setupUi(this);
    // Set the window to open the center of the screen with a fixed size
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->primaryScreen()->availableGeometry()));
    this->setFixedSize(QSize(892, 700));

    QObject::connect(this, &CentralGui::on_send_clicked, this, &CentralGui::send_msg);
    QObject::connect(this, &CentralGui::on_message_txt_returnPressed, this, &CentralGui::send_msg);

    m_menu = new MenuGui();

    m_ring_sound = new QMediaPlayer;
    m_audio_output = new QAudioOutput;

    m_audio_output->setVolume(50);

    m_ui->search->setAutoDefault(false);
    m_ui->add->setAutoDefault(false);
    m_ui->remove->setAutoDefault(false);
    m_ui->call->setAutoDefault(false);

}

CentralGui::~CentralGui()
{
    emit wrapping();
    LOG_INFO("Emitted wrapping");
    
    if (m_menu)
    {
        delete m_menu;
    }

    if (m_audio_output)
    {
        delete m_audio_output;
    }

    if (m_ring_sound)
    {
	    delete m_ring_sound;
    }

    if (m_call) 
    {
      delete m_call;
    }

    delete m_ui;
}

// ***** PUBLIC ***** //

void CentralGui::init()
{
    m_notification = new Notification();
    m_ui->chat_group->hide();
    JobBus::create({Job::GETUSER});
    JobBus::create({Job::LIST});
    TODAY = QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd"));
    YESTERDAY = QDateTime::currentDateTime().addDays(-1).toString(QLatin1String("yyyy-MM-dd"));
}

void CentralGui::reject()
{
    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Closing MySkype", "Are you sure you want to close MySkype?",
                                                            QMessageBox::Ok | QMessageBox::Cancel);

    if (QMessageBox::Ok == ret)
    {
        QDialog::reject();
    }
}

// ***** PRIVATE ***** //

void CentralGui::send_msg()
{
    if (m_ui->message_txt->text() == "")
    {
        return;
    }

    QString time = QDateTime::currentDateTime().toString(QLatin1String("hh:mm"));
    QString user = m_current_selected.data(Qt::DisplayRole).toString();
    auto contact = search_contact_list(user, "username");

    if (0 == contact)
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

void CentralGui::display_chat(QString &t_contact)
{
    auto contact = search_contact_list(t_contact, "username");

    if (contact == 0)
    {
        return;
    }

    m_ui->chat_box->setModel(new QStringListModel(m_contact_chat[contact]));
    m_ui->chat_box->scrollToBottom();
}

void CentralGui::load_chat(QVector<Chat> &chats, bool t_notification)
{
    for (auto &chat : chats)
    {
        std::string time;
        if (TODAY == QString::fromStdString(chat.created_at_date()))
        {
            time = "Today " + chat.created_at_time();
        }
        else if (YESTERDAY == QString::fromStdString(chat.created_at_date()))
        {
            time = "Yesterday " + chat.created_at_time();
        }
        else
        {
            time = chat.created_at_date() + " " + chat.created_at_time();
        }

        if (chat.sender() == m_user_id)
        {
            m_contact_chat[chat.recipient()].append(m_user);
            m_contact_chat[chat.recipient()].append(QString::fromStdString(time) + "\n" + QString::fromStdString(chat.text() + "\n"));
        }
        else
        {
            m_contact_chat[chat.sender()].append(m_contact_list[chat.sender()].username);
            m_contact_chat[chat.sender()].append(QString::fromStdString(time) + "\n" + QString::fromStdString(chat.text() + "\n"));
            if (true == t_notification)
            {
                m_notification->setPopupText((m_contact_list[chat.sender()].username), (QString::fromStdString(chat.text().substr(0, 20))));
                m_notification->show();
            }
        }

        if (false == chat.delivered())
        {
            JobBus::create({Job::DELIVERED, std::to_string(chat.id())});
        }
    }
}

template <typename T>
int CentralGui::search_contact_list(T t_value, QString t_type)
{

    for (auto contact = m_contact_list.begin(); contact != m_contact_list.end(); contact++)
    {
        if (t_type == "online" && t_value.toInt() == contact->online)
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

void CentralGui::on_contact_list_clicked(const QModelIndex &index)
{
    QString user = index.data(Qt::DisplayRole).toString();

    if (index == m_current_selected)
    {
        return;
    }

    m_current_selected = index;
    
    // Search contact list for that specific user to get his ID
    int user_id = search_contact_list(user, "username");
    if (user_id > 0)
    {
        bool status = m_contact_list[user_id].online;
        if (status == true)
        {
            m_ui->call->show();
        }else{
            m_ui->call->hide();
        }
    }

    m_ui->chat_group->show();
    m_ui->contact_txt->setText(user);
    display_chat(user);
}

void CentralGui::on_search_clicked()
{
    m_contact.set_option("Search");
    m_contact.show();
    m_contact.raise();
}

void CentralGui::on_add_clicked()
{
    m_contact.set_option("Add");
    m_contact.show();
    m_contact.raise();
}

void CentralGui::on_remove_clicked()
{
    QString q_user = m_current_selected.data().toString();
    std::string user = q_user.toStdString();

    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, "Remove " + q_user, "Are you sure you want to remove " + q_user + "?",
                                                            QMessageBox::Cancel | QMessageBox::Ok);
    if (QMessageBox::Cancel == ret)
    {
        return;
    }

    JobBus::create({Job::REMOVE, user});
}

void CentralGui::on_call_clicked()
{

    if (true == m_on_call)
    {
        QMessageBox::information(nullptr, "Error", "You are already on a call!", QMessageBox::Ok);
        return;
    }

    m_call = new CallGui(this);
    QString user = m_current_selected.data(Qt::DisplayRole).toString();
    int user_id = search_contact_list(user, "username");

    if (user_id == 0)
    {
        QMessageBox::warning(nullptr, "Error", "Something went wrong while calling " + user + "!\n Please try again later!",
                             QMessageBox::Ok);
        return;
    }

    m_on_call = true;
    m_call->call_init(user_id, user);
}

void CentralGui::on_settings_clicked()
{
    m_menu->refresh_devices();
    m_menu->show();
}

void CentralGui::started_call(int t_caller_id)
{
    if (true == m_on_call)
    {
        QMessageBox::information(nullptr, "Error", "You are already on a call!", QMessageBox::Ok);
        return;
    }

    m_call = new CallGui(this);
    QString user = m_contact_list[t_caller_id].username;
    int user_id = search_contact_list(user, "username");

    if (user_id == 0)
    {
        QMessageBox::warning(nullptr, "Error", "Something went wrong while calling " + user + "!\n Please try again later!",
                             QMessageBox::Ok);
        return;
    }

    m_on_call = true;

    m_ring_sound->stop();
    
    m_ring_sound->setSource(QUrl::fromLocalFile("../misc/rings/answer.mp3"));
    m_ring_sound->play();
   
    m_call->call_accept(user);
}
