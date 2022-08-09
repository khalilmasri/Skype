#include "contacts.hpp"
#include "logger.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

#include <string>
#include <sys/types.h>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>

// /* Public */

bool Contacts::list(ActiveConn& t_conn, Request& t_req) {
    
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid ){
        LOG_ERR("Listing request failed");
    }
    
    return t_req.m_valid;
}

bool Contacts::search(ActiveConn& t_conn, Request& t_req){
    
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid ){
        LOG_ERR("Search request failed");
    }
    
    return t_req.m_valid; 
}

bool Contacts::add_user(ActiveConn& t_conn, Request& t_req){

    std::string user = StringUtils::last(TextData::to_string(t_req.data()));
    
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid || user == ""){
        LOG_ERR("Add user request failed");
        return t_req.m_valid;  
    }
    
    std::string response = TextData::to_string(t_req.data());
    bool ret = valid_response(Reply::r_200, response);
    
    if ( false == ret ) {
        LOG_INFO("Add user failed, server response => %s", response.c_str());
        return false;
    }

    LOG_INFO("Added user %s", user.c_str());
    
    m_contacts.push_back(user);
    return t_req.m_valid;
}

bool Contacts::remove_user(std::string& t_cmd){

    std::string user = StringUtils::last(t_cmd);

    auto found = std::find(m_contacts.begin(), m_contacts.end(), user);
    
    if ( found != m_contacts.end()) {
        m_contacts.erase(found);
        LOG_INFO("Removed user %s", user.c_str());
        return true;
    }
    
    return false;
}

bool Contacts::available(ActiveConn& t_conn, Request& t_req){

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid ){
        LOG_ERR("Availabe request failed");
    }
    
    std::cout << TextData::to_string(t_req.data()) << std::endl;

    return t_req.m_valid;
}

std::vector<std::string> Contacts::get_contacts() const {
    return m_contacts;
}

void Contacts::set_current_contact(std::string &t_current_contact){
    m_current_contact = t_current_contact;
}

std::string Contacts::get_current_contact(){
    return m_current_contact;
}

// /* Private */

bool Contacts::valid_response(Reply::Code t_code, std::string& t_res){

    std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}
