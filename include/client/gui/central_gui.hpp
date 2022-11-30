#ifndef CHAT_GUI_HPP
#define CHAT_GUI_HPP

#include "job_bus.hpp"
#include "job.hpp"
#include "contact_gui.hpp"
#include "contacts.hpp"
#include "chat.hpp"
#include "notification.hpp"
#include "call_gui.hpp"

#include <QModelIndex>
#include <QDialog>
#include <QVector>
#include <QString>
#include <QStringListModel>
#include <string>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class CentralGui;
}

class CentralGui : public QDialog
{
    Q_OBJECT

public:
    explicit CentralGui(QWidget *parent = nullptr);
    ~CentralGui();

    void init();
    void remove_user(QString t_user);

    void job_disp_contact(Job &t_job);
    void job_set_user(Job &t_job);
    void job_add_user(Job &t_job);
    void job_search(Job &t_job);
    void job_remove_user(Job &t_job);
    void job_load_chat(Job &t_job);
    void job_load_pending(Job &t_job);
    void job_set_id(Job &t_job);
    void job_send_msg(Job &t_job);
    void job_hangup(Job &t_job);
    void job_awaiting(Job &t_job);
    void job_video_stream(Job &t_job);
    void job_video_failed(Job &t_job);
    void job_audio_failed(Job &t_job);

private slots:
    void on_contact_list_clicked(const QModelIndex &index);
    void on_add_clicked();
    void on_search_clicked();
    void on_remove_clicked();
    void on_call_clicked();
    void on_video_clicked();

public slots:
    void started_call(int t_caller_id);

signals:
    void on_send_clicked();
    void wrapping();
    void on_message_txt_returnPressed();
    void ready_signal();

private: // Variables
    Ui::CentralGui                  *m_ui;
    CallGui                         *m_call;
    QString                         m_user;
	 QMediaPlayer							*m_ring_sound;
	 QAudioOutput							*m_audio_output;
    int                             m_user_id;
    QModelIndex                     m_current_selected;
    ContactGui                      m_contact;
    Notification                    *m_notification;
    QHash<int, struct Details>      m_contact_list;
    QHash<int, QStringList>         m_contact_chat;
    bool                            m_on_call = false;

private: // Methods
    void reject() override;
    void refresh();
    void load_chat(QVector<Chat> &chats, bool t_notification);
    void send_msg();
    void display_chat(QString &t_user);
    template <typename T>
    int search_contact_list(T t_value, QString t_type);
};

#endif // CHAT_HPP
