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

    void load_contacts(QVector<QString> t_contact_list);
    void set_user(QString t_user);
    void init();
    void remove_user(QString t_user);

private slots:
    void on_contact_list_clicked(const QModelIndex &index);

    void on_message_txt_returnPressed();

    void on_send_clicked();

    void on_search_clicked();

    void on_add_clicked();

    void on_remove_clicked();

private: // Variables
    Ui::ChatGui *m_ui;
    QString     m_user;
    QModelIndex m_current_selected;
    ContactGui  m_contact;

private: // Methods
    void refresh_contacts();
    void load_chat(QString t_contact);
    void send_msg();
};

#endif // CHAT_HPP
