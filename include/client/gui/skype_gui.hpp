#ifndef SKYPEGUI_H
#define SKYPEGUI_H

#include "chat.hpp"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SkypeGui; }
QT_END_NAMESPACE

class SkypeGui : public QMainWindow
{
    Q_OBJECT

public:
    SkypeGui(QWidget *parent = nullptr);
    ~SkypeGui();

private slots:
    void on_pushButton_login_window_clicked();

private slots:
    void on_pushButton_register_window_clicked();

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_register_clicked();

private:
    Ui::SkypeGui *m_welcome_ui;
    Chat *m_chat_ui;
    QThread *m_job_loop;
    bool m_exit;
    void job_loop();
};
#endif // SKYPEGUI_H
