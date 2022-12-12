#ifndef JOB_H
#define JOB_H

#include "chat.hpp"
#include "contacts.hpp"
#include "webcam.hpp"
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
        AUDIO_FAILED,
        VIDEO_FAILED,
        CLEANUP,  // cleans up call object oncall failure
        PEER_HANGUP,  // Response from Call when peer hangsup
                  
        // other
        EXIT,
        NONE,
    };
    using VideoQueuePtr = std::shared_ptr<Webcam::CVMatQueue>;
    using ContactListMap = QHash<int, struct Details>;

    Type           m_command;
    std::string    m_argument     = "";
    bool           m_valid        = false;
    bool           m_boolValue    = false;
    std::string    m_string       = "";
    int            m_intValue     = -1;
    QString        m_qstring      = "";
    ContactListMap m_contact_list = {};
    QVector<Chat>  m_chats        = {};
    VideoQueuePtr  m_video_stream = nullptr; 
};


#endif // JOB_H
