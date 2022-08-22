#include "skype_gui.hpp"
#include "ui/ui_skype_gui.h"
#include "job_bus.hpp"
#include "job.hpp"
#include "fail_if.hpp"

#include <QMessageBox>
#include <QThread>
#include <iostream>

SkypeGui::SkypeGui(QWidget *parent)
    : QMainWindow(parent)
    , m_welcome_ui(new Ui::SkypeGui)
{
    m_welcome_ui->setupUi(this);
    this->setFixedSize(QSize(621, 473));
    m_welcome_ui->Register_group->hide();

    m_exit = false;

    auto lm = [this]{job_loop();};
    m_job_loop = QThread::create(lm);
    m_job_loop->start();
}

SkypeGui::~SkypeGui()
{
    delete m_welcome_ui;
    m_exit = true;
    m_job_loop->wait();
}

void SkypeGui::job_loop(){

    while(m_exit == false){
        Job job;
        JobBus::get_response(job);
        if (job.m_command == Job::LOGIN && job.m_valid == true){
            m_chat_ui = new Chat(this);
            hide();
            m_chat_ui->show();
        }

    }
}


void SkypeGui::on_pushButton_login_clicked()
{
    QString username = m_welcome_ui->lineEdit_login_username->text();
    QString password = m_welcome_ui->lineEdit_login_password->text();

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::LOGIN});
}

void SkypeGui::on_pushButton_register_window_clicked()
{
    m_welcome_ui->Login_group->hide();
    m_welcome_ui->Register_group->show();
}

void SkypeGui::on_pushButton_login_window_clicked()
{
    m_welcome_ui->Register_group->hide();
    m_welcome_ui->Login_group->show();
}


void SkypeGui::on_pushButton_register_clicked()
{
    QString username = m_welcome_ui->lineEdit_register_username->text();
    QString password = m_welcome_ui->lineEdit_register_password->text();
    QString confirm_password = m_welcome_ui->lineEdit_register_confirm_password->text();

    FAIL_IF(confirm_password != password);

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::CREATE});

    return;

fail:
    QMessageBox::information(this, "register", "Password doesn't match");
}

