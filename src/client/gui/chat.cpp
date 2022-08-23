#include "chat.hpp"
#include "ui/ui_chat.h"

#include <QStringListModel>
#include <QVector>
#include <QList>
#include <QString>
#include <QTimer>
#include <QDebug>

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

void ChatGui::init(){
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
}

void ChatGui::refresh_contacts()
{
    if (first == true ){
        connect(timer, &QTimer::timeout, this, &ChatGui::refresh_contacts);
        timer->start(10000);
        first = false;
    }

    JobBus::handle({Job::LIST});
    JobBus::handle({Job::DISP_CONTACTS});
}

void ChatGui::on_contact_list_clicked(const QModelIndex &index)
{
    QVariant item = index.data(Qt::DisplayRole);
    m_ui->contact_txt->setText(item.toString());
}
