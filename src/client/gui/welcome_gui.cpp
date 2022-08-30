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
    QObject::connect(this, &WelcomeGui::on_pushButton_login_clicked,                             this, &WelcomeGui::login);
    QObject::connect(this, &WelcomeGui::on_lineEdit_login_username_returnPressed,                this, &WelcomeGui::login);
    QObject::connect(this, &WelcomeGui::on_lineEdit_login_password_returnPressed,                this, &WelcomeGui::login);
    QObject::connect(this, &WelcomeGui::on_pushButton_register_clicked,                          this, &WelcomeGui::create);
    QObject::connect(this, &WelcomeGui::on_lineEdit_register_username_returnPressed,             this, &WelcomeGui::create);
    QObject::connect(this, &WelcomeGui::on_lineEdit_register_password_returnPressed,             this, &WelcomeGui::create);
    QObject::connect(this, &WelcomeGui::on_lineEdit_register_confirm_password_returnPressed,     this, &WelcomeGui::create);
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
        QMessageBox::information(nullptr, "Registeration", "Couldn't register account\nUsername is used!");
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
    m_ui->login_error->setText("");

    FAIL_IF( false == check_input(username, password));

    JobBus::create({Job::SETUSER, username.toStdString()});
    JobBus::create({Job::SETPASS, password.toStdString()});
    JobBus::create({Job::LOGIN});

    return;
fail:

    m_ui->login_error->setText("Invalid credentials\nMake sure that the username and password\nare minimum 3 characters!");
}

void WelcomeGui::create()
{
    QString username = m_ui->lineEdit_register_username->text();
    QString password = m_ui->lineEdit_register_password->text();
    QString confirm_password = m_ui->lineEdit_register_confirm_password->text();
    m_ui->register_error->setText("");

    FAIL_IF(confirm_password != password);

    if ( false == check_input(username, password))
    {
        m_ui->register_error->setText("Invalid credentials\nMake sure that the username and password\nare minimum 3 characters!");
        return;
    }

    JobBus::create({Job::SETUSER, username.toStdString()});
    JobBus::create({Job::SETPASS, password.toStdString()});
    JobBus::create({Job::CREATE});

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

bool WelcomeGui::check_input(QString t_username, QString t_password)
{

    if ( "" == t_username || "" == t_password)
    {
        return false;
    }

    if ( MIN_LEN > t_password.size() || MIN_LEN > t_username.size())
    {
        return false;
    }

    return true;
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
