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
    std::string response = "";
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF( false == t_req.m_valid);
    response = TextData::to_string(t_req.data());

    LOG_INFO("Updating contacts");    
    update_contacts(response);
    
    return true;

fail:
    return false;
}

bool Contacts::search(ActiveConn& t_conn, Request& t_req){
    
    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF ( user == "" );

    command = "SEARCH "+ user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid); 
    
    response = TextData::to_string(t_req.data());
    
    FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("Searched database and found user %s!", user.c_str());
    
    return true;
fail:
    return false;
}

bool Contacts::add_user(ActiveConn& t_conn, Request& t_req){

    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF ( user == "" );

    command = "ADD "+ user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid); 
    
    response = TextData::to_string(t_req.data());
    
    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

    LOG_INFO("Added user %s", user.c_str());
    
    list(t_conn, t_req);
    LOG_INFO("Added new user and updated contacts");
    
    return true;

fail:
    return false;
}

bool Contacts::remove_user(ActiveConn& t_conn, Request& t_req){

    std::string response = "";
    std::string command = "" ;
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF ( user == "" );
    
    command = "REMOVE "+ user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

    list(t_conn, t_req);
    LOG_INFO("Removed user and updated contacts");

    return true;

fail:
    return false;
}

bool Contacts::available(ActiveConn& t_conn, Request& t_req){

    std::string response = "";
    std::string command = "" ;
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF ( user == "" );

    command = "AVAILABLE " + user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF (false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("User %s is available!", user.c_str());

    return true;
fail:
    return false;
}

std::vector<std::string> Contacts::display_contacts() {

    std::vector<std::string> contacts;

    FAIL_IF ( true == m_online_contacts.empty() );

    for ( auto &[username, details] : m_online_contacts ) {
        contacts.push_back(username);
    }

    return contacts;

fail:
    return {};
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

void Contacts::update_contacts(std::string t_response) {

    StringUtils::StringVector users = StringUtils::split(t_response);

    for ( auto &user : users) {
        pair_contact_details(user); 
    }
}

void Contacts::pair_contact_details(std::string t_user) {
     
    StringUtils::StringVector user_fields = StringUtils::split(t_user, ",");

    Details details;
    std::string username = "";
    
    for(auto field : user_fields) {
        
        auto [key, pair] = StringUtils::split_first(field, ":");
        
        if ( key == "id" ) {
            details.ID = stoi(pair);
        }

        if ( key == "username" ) {
            username = pair;
        }

        if ( key == "online" ) {
            if ( pair == "true" ){
                details.online = true;
            }else{
                details.online = false;
            }
        }

        if ( key == "address" ) {
            details.address = pair;
        }
    }

    if ( username != "" && details.online == true ){
        m_online_contacts.emplace(username, details);            
    }
}