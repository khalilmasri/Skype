#include "welcome_gui.hpp"
#include "ui/ui_welcome.h"
#include "job.hpp"
#include "job_bus.hpp"
#include "fail_if.hpp"

#include <QMessageBox>
#include <QDesktopWidget>
#include <QStyle>
#include <QLineEdit>

WelcomeGui::WelcomeGui(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::WelcomeGui)
{
    m_ui->setupUi(this);

    // Set the window to open the center of the screen with a fixed size
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),
                        qApp->desktop()->availableGeometry()));
    
    this->setFixedSize(QSize(621, 473));

    m_ui->Register_group->hide();

    // Register all the signals to slots
    connect(this, &WelcomeGui::on_pushButton_login_clicked,                             this, &WelcomeGui::login);
    connect(this, &WelcomeGui::on_lineEdit_login_username_returnPressed,                this, &WelcomeGui::login);
    connect(this, &WelcomeGui::on_lineEdit_login_password_returnPressed,                this, &WelcomeGui::login);
    connect(this, &WelcomeGui::on_pushButton_register_clicked,                          this, &WelcomeGui::create);
    connect(this, &WelcomeGui::on_lineEdit_register_username_returnPressed,             this, &WelcomeGui::create);
    connect(this, &WelcomeGui::on_lineEdit_register_password_returnPressed,             this, &WelcomeGui::create);
    connect(this, &WelcomeGui::on_lineEdit_register_confirm_password_returnPressed,     this, &WelcomeGui::create);
}

WelcomeGui::~WelcomeGui()
{
    delete m_ui;
}


// ***** PUBLIC ***** //

void WelcomeGui::job_create(Job &t_job)
{
      if ( false == t_job.m_valid)
    {
        QMessageBox::information(nullptr, "Registeration", "Couldn't register account!");
        return;
    }

    QMessageBox::information(nullptr, "register", "Account creation was success!");
    hide_group("register");
}
  
// ***** PRIVATE ***** //

void WelcomeGui::login()
{
    QString username = m_ui->lineEdit_login_username->text();
    QString password = m_ui->lineEdit_login_password->text();

    if ( "" == password && "" == username)
    {
        return;
    }

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::LOGIN});
}

void WelcomeGui::create()
{
    QString username = m_ui->lineEdit_register_username->text();
    QString password = m_ui->lineEdit_register_password->text();
    QString confirm_password = m_ui->lineEdit_register_confirm_password->text();

    FAIL_IF(confirm_password != password);

    if ( "" == password && "" == username)
    {
        return;
    }

    JobBus::handle({Job::SETUSER, username.toStdString()});
    JobBus::handle({Job::SETPASS, password.toStdString()});
    JobBus::handle({Job::CREATE});

    return;

fail:

    QMessageBox::information(nullptr, "Registeration", "Passwords don't match");
}

void WelcomeGui::hide_group(QString t_group)
{
    if ( "register" == t_group )
    {
        m_ui->Register_group->hide();
        m_ui->Login_group->show();
    }
    else
    {
        m_ui->Login_group->hide();
        m_ui->Register_group->show();
    }
}

// ***** SLOTS ***** //

void WelcomeGui::on_pushButton_register_window_clicked()
{
    hide_group("login");
}

void WelcomeGui::on_pushButton_login_window_clicked()
{
    hide_group("register");
}
