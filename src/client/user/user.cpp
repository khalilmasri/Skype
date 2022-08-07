#include "user.hpp"
#include "logger.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

/* Public */

bool User::register_user(ActiveConn& t_conn, Request& t_req) {
    
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid){
        LOG_ERR("Register request failed");
    }

    std::string response = TextData::to_string(t_req.data());
    bool ret = valid_response(Reply::r_200, response);

    if ( false == ret ){
        LOG_INFO("Register user failed, server response => %s", response.c_str());
        return false;
    }

    LOG_INFO("Register user %s successful", m_username.c_str());
    
    return t_req.m_valid; 
}

bool User::login(ActiveConn& t_conn, Request& t_req) {

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if ( false == t_req.m_valid ){
        LOG_ERR("Login request failed");
        return t_req.m_valid;
    }
  
    std::string response = TextData::to_string(t_req.data());
    bool ret = valid_response(Reply::r_200, response);

    if ( false == ret ){
        LOG_INFO("Login failed, server response => %s", response.c_str());
        return false;
    }

    LOG_INFO("Login to user %s was successful", m_username.c_str());
    m_logged_in = true;
    m_password = "";

    return t_req.m_valid; 
}

bool User::set_username(std::string& t_username) {

    if ( t_username == "" ){
        LOG_ERR("Invalid username");
        return false;
    }

    m_username = t_username;

    return true;
}

bool User::set_password(std::string& t_password) {

    if ( t_password == "" ) {
        LOG_ERR("Invalid password");
        return false;
    } 

    m_password = t_password;

    return true;
}

std::string User::get_username() const {
    return m_username;
}

bool User::get_logged_in() const {
    return m_logged_in;
}

/* Private */
bool User::valid_response(Reply::Code t_code, std::string& t_res){

    std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}
