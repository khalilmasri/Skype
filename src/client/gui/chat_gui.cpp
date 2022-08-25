#include "chat_gui.hpp"
#include "ui/ui_chat.h"
#include "logger.hpp"
#include "fail_if.hpp"

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

bool first = true;
QTimer *timer = new QTimer();

ChatGui::ChatGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChatGui)
{
    m_ui->setupUi(this);

    // Set the window to open the center of the screen with a fixed size
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),qApp->desktop()->availableGeometry()));
    this->setFixedSize(QSize(892, 700));

    connect(this, &ChatGui::on_send_clicked,                this, &ChatGui::send_msg);
    connect(this, &ChatGui::on_message_txt_returnPressed,   this, &ChatGui::send_msg);
}

ChatGui::~ChatGui()
{
    m_contact.hide();
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

    m_ui->contact_list->setModel(new QStringListModel(QList<QString>::fromVector(t_job.m_vector)));

    if (m_current_selected.isValid() != true)
    {
        return;
    }

    m_ui->contact_list->selectionModel()->select(m_current_selected,  QItemSelectionModel::Select);
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
    JobBus::handle({Job::DISP_CONTACTS});
    m_contact.hide();

    //m_current_selected = m_ui->contact_list->currentIndex();
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
}

// ***** PRIVATE ***** //

void ChatGui::refresh_contacts()
{
    if (first == true ){
        connect(timer, &QTimer::timeout, this, &ChatGui::refresh_contacts);
        timer->start(3000);
        first = false;
    }

    JobBus::handle({Job::LIST});
    JobBus::handle({Job::DISP_CONTACTS});
}

void ChatGui::load_chat(QString t_contact)
{  
    QString path = "../chat_logs/" + t_contact + ".txt";

    QFile chat_file(path);

    if (false == chat_file.open(QFile::ReadOnly))
    {
        // TODO: Later we need to add that we need to create a new chat file for the user
        m_ui->chat_box->clear();
        return;
    }

    QTextStream in(&chat_file);
    m_ui->chat_box->clear();

    while( false == in.atEnd()){
        m_ui->chat_box->addItem((in.readLine()));
    }
}

void ChatGui::send_msg()
{
    if(m_ui->message_txt->text() == ""){
        return;
    }

    QString time = QDateTime::currentDateTime().toString(QLatin1String("hh:mm"));

    m_ui->chat_box->addItem(("\n" + time + " " + m_user + ":"));
    m_ui->chat_box->addItem((m_ui->message_txt->text()));
    m_ui->message_txt->setText("");
    m_ui->chat_box->scrollToBottom();
}

// ***** SLOTS ***** //

void ChatGui::on_contact_list_clicked(const QModelIndex &index)
{
    QString item = index.data(Qt::DisplayRole).toString();

    if (index == m_current_selected){
        return;
    }

    m_ui->chat_group->show();
    m_current_selected = index;
    m_ui->contact_txt->setText(item);
    load_chat(item);
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

/*
QSaveFile fileOut(filename);
QTextStream out(&fileOut);
out << "Qt rocks!" << Qt::endl; // do this for every item
fileOut.commit()
*/









