#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class WelcomeGui; }
QT_END_NAMESPACE

class WelcomeGui : public QMainWindow
{
    Q_OBJECT

public:
    WelcomeGui(QWidget *parent = nullptr);
    ~WelcomeGui();

private slots:
    void on_pushButton_login_window_clicked();

private slots:
    void on_pushButton_register_window_clicked();

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_register_clicked();

private:
    Ui::WelcomeGui *m_ui;
};

#endif // WELCOME_H
