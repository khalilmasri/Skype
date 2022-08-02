#ifndef SESSION_H
#define SESSION_H

#include "user.hpp"
#include "contacts.hpp"

#include <iostream>
#include <string>

class Session {

public:

    bool set_credentials( const User& );
    void update_contacts( const Contacts& );

    void render_contacts();
    void render_credentials();
    
private:
    std::string m_username;
    std::vector<std::string&> m_contact_list;
};

#endif // SESSION_H