#ifndef JOB_H
#define JOB_H

#include <string>
#include <vector>
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

        // std::string return from 20 - 29
        GETUSER         = 20,

        // std::vector return values from 30 - 39
        DISP_CONTACTS   = 30,
        
        // None
        NONE            = 50,
    };

    Type m_command;
    std::string m_argument = "";

    bool m_valid = false;
    std::string m_string = "";
    std::vector<std::string> m_vector = {};
};



#endif // JOB_H