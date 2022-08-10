#ifndef JOB_H
#define JOB_H

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

        // Login details
        SETUSER,
        SETPASS,
        CREATE,
        LOGIN,
        LOGGED,
        NONE,
    };

    Type command;
    std::string argument;
};



#endif // JOB_H