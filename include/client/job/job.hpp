#ifndef JOB_H
#define JOB_H

#include "chat.hpp"
#include "contacts.hpp"

#include <QString>
#include <QVector>
#include <string>

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
        ACCEPT,
        REJECT, 
        HANGUP,
        WEBCAM,
        MUTE,
        
        DISCARD,
        NONE,
    };

    Type m_command;
    std::string m_argument = "";

    bool m_valid = false;
    std::string m_string = "";
    int m_intValue = -1;
    QHash<int, struct Details> m_contact_list = {};
    QVector<Chat> m_chats = {};
    QString m_qstring = "";
};



#endif // JOB_H
