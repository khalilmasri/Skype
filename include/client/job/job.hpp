#ifndef JOB_H
#define JOB_H

#include "chat.hpp"
#include "contacts.hpp"
#include "video_playback.hpp"
#include "thread_safe_queue.hpp"

#include <QString>
#include <QVector>
#include <string>
#include <memory>

struct Job {
     enum Type {
        // Contacts
        LIST,
        SEARCH,
        ADD,
        REMOVE,
        AVAILABLE,
        DISP_CONTACTS,

        // Accounts
        SETUSER,      
        SETPASS,       
        CREATE,        
        LOGIN,         
        LOGGED,          
        GETUSER,
        GETID,

        // Chat
        SEND,
        CHAT,
        PENDING,
        DELIVERED,
        NOTIFICATION,
        
        // Call
        CONNECT,
        AWAITING,
        AWAITING_NOTIFY,
        ACCEPT,
        REJECT, 
        HANGUP,
        WEBCAM,
        DISCARD,
        VIDEO_STREAM, // this returns a video queue stream back to the GUI
        EXIT,
        NONE,
    };


    Type m_command;
    std::string m_argument = "";

    bool m_valid = false;
    bool m_boolValue = false;
    std::string m_string = "";
    int m_intValue = -1;
    QHash<int, struct Details> m_contact_list = {};
    QVector<Chat> m_chats = {};
    QString m_qstring = "";
    VideoPlayback::VideoQueuePtr m_video_stream = nullptr; // this is an unique ptr
};



#endif // JOB_H
