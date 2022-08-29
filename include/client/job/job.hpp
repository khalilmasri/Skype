#ifndef JOB_H
#define JOB_H

#include "chat.hpp"
#include "contacts.hpp"

#include <QString>
#include <QVector>
#include <string>

struct Job {
     enum Type {

        // bool return values from 0 - 19
        LIST            = 0,
        SEARCH          = 1,
        ADD             = 2,
        REMOVE          = 3,
        AVAILABLE       = 4,
        SETUSER         = 5,
        SETPASS         = 6,
        CREATE          = 7,
        LOGIN           = 8,
        LOGGED          = 9,

        // string return from 20 - 29
        GETUSER         = 20,
        SEND            = 21,

        // vector return values from 30 - 39
        DISP_CONTACTS   = 30,
        CHAT            = 31,
        
        // None
        NONE            = 50,
    };

    Type m_command;
    std::string m_argument = "";

    bool m_valid = false;
    std::string m_string = "";
    QHash<int, QString> m_contact_list = {};
    QVector<Chat> m_chats = {};
    QString m_time = "";
};



#endif // JOB_H
