#ifndef CHAT_HPP
#define CHAT_HPP

#include "job_bus.hpp"
#include "job.hpp"

#include <QModelIndex>
#include <QDialog>

namespace Ui {
class ChatGui;
}

class ChatGui : public QDialog
{
    Q_OBJECT

public:
    explicit ChatGui(QWidget *parent = nullptr);
    ~ChatGui();

    void load_contacts(Job &t_job);
    void set_user(Job &t_job);

    void refresh_contacts();
    void init();

private slots:
    void on_contact_list_clicked(const QModelIndex &index);

    void on_message_txt_returnPressed();

    void on_send_clicked();

private:
    Ui::ChatGui *m_ui;
    QString     m_user;
    QModelIndex m_current_selected;

    void load_chat(QString t_contact);
    void send_msg();
};

#endif // CHAT_HPP
