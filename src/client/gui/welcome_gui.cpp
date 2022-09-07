#include "welcome_gui.hpp"
#include "ui/ui_welcome.h"
#include "job.hpp"
#include "job_bus.hpp"
#include "fail_if.hpp"
#include "logger.hpp"

#include <QMessageBox>
#include <QStyle>
#include <QLineEdit>
#include <QScreen>
#include <QWindow>
#include <QMovie>
#include <QLabel>
#include <QTimer>

WelcomeGui::WelcomeGui(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::WelcomeGui)
{
    m_ui->setupUi(this);

    // Set the window to open the center of the screen with a fixed size
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),
                        qApp->primaryScreen()->availableGeometry()));
    
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

void WelcomeGui::load_screen()
{
    m_ui->Login_group->hide();
    m_ui->Register_group->hide();
    m_loading_gif = new QMovie("../misc/gif/loading.gif");
    m_ui->loading->setMovie(m_loading_gif);
    m_loading_gif->start();
}

void WelcomeGui::stop_loading()
{ 
    QTimer::singleShot(1500,m_loading_gif,SLOT(stop()));
    QTimer::singleShot(1500,this, SIGNAL(stopped_loading()));
    LOG_INFO("Loading complete!");
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

void WelcomeGui::hide_group(QString t_group)
{
    if ( "register" == t_group )
    {
        m_ui->Register_group->hide();
        m_ui->Login_group->show();
    }
    else if ( "both" == t_group )
    {
        
    }else
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
