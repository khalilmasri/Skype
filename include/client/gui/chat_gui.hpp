#ifndef CHAT_HPP
#define CHAT_HPP

#include "job_bus.hpp"
#include "job.hpp"
#include "contact_gui.hpp"

#include <QModelIndex>
#include <QDialog>
#include <QVector>
#include <QString>

namespace Ui {
class ChatGui;
}

class ChatGui : public QDialog
{
    Q_OBJECT

public:
    explicit ChatGui(QWidget *parent = nullptr);
    ~ChatGui();

    void init();
    void remove_user(QString t_user);

    void job_disp_contact(Job &t_job);
    void job_set_user(Job &t_job);
    void job_add_user(Job &t_job);
    void job_search(Job &t_job);
    void job_remove_user(Job &t_job);

private slots:
    void on_contact_list_clicked(const QModelIndex &index);
    void on_add_clicked();
    void on_search_clicked();
    void on_remove_clicked();

signals:
    void on_send_clicked();
    void on_message_txt_returnPressed();

private: // Variables
    Ui::ChatGui *m_ui;
    QString     m_user;
    QModelIndex m_current_selected;
    ContactGui  m_contact;

private: // Methods
    void reject() override;
    void refresh_contacts();
    void load_chat(QString t_contact);
    void send_msg();
};

#endif // CHAT_HPP
