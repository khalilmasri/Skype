#include "user.hpp"
#include "logger.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "server_commands.hpp"
#include "fail_if.hpp"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

/* Public */

bool User::register_user(ActiveConn& t_conn, Request& t_req) {
    
    std::string response = "";
    std::string command = "CREATE " + m_username + " " + m_password; 
    t_req.set_data(new TextData(command));
    
    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);
    
    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

    LOG_INFO("Register user %s successful", m_username.c_str());
    
    return t_req.m_valid; 
    
fail:
    return false;
}

bool User::login(ActiveConn& t_conn, Request& t_req) {

    std::string response = "";
    std::string command = "LOGIN " + m_username + " " + m_password; 
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);
    
    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_200, response), response.c_str());

    LOG_INFO("Login to user %s was successful", m_username.c_str());
    
    m_logged_in = true;
    m_password = "";
    
    return t_req.m_valid; 
fail:
    return false;
}

bool User::set_username(std::string& t_username) {

    m_username = t_username;

    return true;
}

bool User::set_password(std::string& t_password) {

    m_password = t_password;

    return true;
}

std::string User::get_username(){
    return m_username;
}

bool User::get_logged_in(){
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
