#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>
#include <job.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class WelcomeGui; }
QT_END_NAMESPACE

class WelcomeGui : public QMainWindow
{
    Q_OBJECT

public:
    WelcomeGui(QWidget *parent = nullptr);
    ~WelcomeGui();

    void job_create(Job &t_job);

private slots:
    void on_pushButton_login_window_clicked();
    void on_pushButton_register_window_clicked();

signals:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void on_lineEdit_login_username_returnPressed();
    void on_lineEdit_login_password_returnPressed();
    void on_lineEdit_register_username_returnPressed();
    void on_lineEdit_register_password_returnPressed();
    void on_lineEdit_register_confirm_password_returnPressed();

private: // Variables
    Ui::WelcomeGui *m_ui;

private: // Methods
    void login();
    void create();
    void hide_group(QString t_group);
};

#endif // WELCOME_H
