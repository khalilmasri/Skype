#include "contacts.hpp"
#include "logger.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "fail_if.hpp"

#include <string>
#include <sys/types.h>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>

// /* Public */

bool Contacts::list(ActiveConn& t_conn, Request& t_req) {
    
    std::string command = "LIST";
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF( false == t_req.m_valid);
    
    std::cout << TextData::to_string(t_req.data()) << std::endl;
    return t_req.m_valid;

fail:
    return false;
}

bool Contacts::search(ActiveConn& t_conn, Request& t_req){
    
    std::string response = "";
    std::string command = "";
    std::string user = StringUtils::last(TextData::to_string(t_req.data()));
    FAIL_IF ( user == "" );

    command = "SEARCH "+ user;
    t_req.set_data(new TextData(command));
    std::cout << command << std::endl;
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid); 
    
    response = TextData::to_string(t_req.data());
    
    std::cout << response << std::endl;
    FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("Searched database and found user %s! server response => %s", user.c_str(), response.c_str());
    
    return true;
fail:
    return false;
}

bool Contacts::add_user(ActiveConn& t_conn, Request& t_req){

    std::string response = "";
    std::string command = "";
    std::string user = StringUtils::last(TextData::to_string(t_req.data()));
    FAIL_IF ( user == "" );

    command = "ADD "+ user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid); 
    
    response = TextData::to_string(t_req.data());
    
    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

    LOG_INFO("Added user %s", user.c_str());
    
    m_contacts.push_back(user);
    
    return t_req.m_valid;

fail:
    return false;
}

bool Contacts::remove_user(ActiveConn& t_conn, Request& t_req){

    std::string response = "";
    std::string command = "" ;
    std::string user = StringUtils::last(TextData::to_string(t_req.data()));
    FAIL_IF ( user == "" );
    
    command = "REMOVE "+ user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());
    {
        auto found = std::find(m_contacts.begin(), m_contacts.end(), user);
        
        FAIL_IF ( found != m_contacts.end() );
    
        m_contacts.erase(found);
        LOG_INFO("Removed user %s", user.c_str());
    }

    return true;

fail:
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

// /* Private */

bool Contacts::valid_response(Reply::Code t_code, std::string& t_res){

    std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}
