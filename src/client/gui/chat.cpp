#include "chat.hpp"
#include "ui/ui_chat.h"
#include "logger.hpp"

#include <QStringListModel>
#include <QVector>
#include <QList>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QDateTime>
#include <QTextStream>

bool first = true;
QTimer *timer = new QTimer();

ChatGui::ChatGui(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChatGui)
{
    m_ui->setupUi(this);
}

ChatGui::~ChatGui()
{
    delete m_ui;
    delete timer;
}

void ChatGui::init()
{
    m_ui->chat_group->hide();
    refresh_contacts();
    JobBus::handle({Job::GETUSER});
}

void ChatGui::set_user(Job &t_job){
    m_user = QString::fromUtf8(t_job.m_string.c_str());
    m_ui->username->setText("Welcome " + m_user);
}

void ChatGui::load_contacts(Job &t_job)
{
    m_ui->contact_list->setModel(new QStringListModel(QList<QString>::fromVector(t_job.m_vector)));
    m_ui->contact_list->selectionModel()->select(m_current_selected,  QItemSelectionModel::Select);
}

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

void ChatGui::on_contact_list_clicked(const QModelIndex &index)
{
    QString item = index.data(Qt::DisplayRole).toString();

    if (item == m_ui->contact_txt->text()){
        return;
    }

    m_ui->chat_group->show();
    m_current_selected = index;
    m_ui->contact_txt->setText(item);
    load_chat(item);
}

void ChatGui::load_chat(QString t_contact)
{
    QString path = "../../chat_logs/" + t_contact + ".txt";

    QFile chat_file(path);
    chat_file.open(QFile::ReadOnly);

    QTextStream in(&chat_file);

    m_ui->chat_box->clear();

    while( false == in.atEnd()){
        m_ui->chat_box->addItem((in.readLine()));
    }
}

void ChatGui::on_message_txt_returnPressed()
{
    send_msg();
}

void ChatGui::on_send_clicked()
{
    send_msg();
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

/*
QSaveFile fileOut(filename);
QTextStream out(&fileOut);
out << "Qt rocks!" << Qt::endl; // do this for every item
fileOut.commit()
*/
