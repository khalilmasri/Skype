#include "welcome.hpp"
#include "ui/ui_welcome.h"
#include "job.hpp"
#include "job_bus.hpp"
#include "fail_if.hpp"

#include <QMessageBox>

WelcomeGui::WelcomeGui(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::WelcomeGui)
{
    m_ui->setupUi(this);
    this->setFixedSize(QSize(621, 473));
    m_ui->Register_group->hide();
}

WelcomeGui::~WelcomeGui()
{
    delete m_ui;
}

void WelcomeGui::on_pushButton_login_clicked()
{
    QString username = m_ui->lineEdit_login_username->text();
    QString password = m_ui->lineEdit_login_password->text();

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::LOGIN});
}

void WelcomeGui::on_pushButton_register_window_clicked()
{
    m_ui->Login_group->hide();
    m_ui->Register_group->show();
}

void WelcomeGui::on_pushButton_login_window_clicked()
{
    m_ui->Register_group->hide();
    m_ui->Login_group->show();
}


void WelcomeGui::on_pushButton_register_clicked()
{
    QString username = m_ui->lineEdit_register_username->text();
    QString password = m_ui->lineEdit_register_password->text();
    QString confirm_password = m_ui->lineEdit_register_confirm_password->text();

    FAIL_IF(confirm_password != password);

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::CREATE});

    return;

fail:
    QMessageBox::information(this, "register", "Password doesn't match");
}

