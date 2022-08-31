#include "accounts.hpp"
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

bool Accounts::register_user(ActiveConn& t_conn, Request& t_req) {
    
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

bool Accounts::login(ActiveConn& t_conn, Request& t_req) {

    std::string response = "";
    std::string command = "LOGIN " + m_username + " " + m_password;
    StringUtils::StringTuple parse; 
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);
    
    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG( false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("Login to user %s was successful", m_username.c_str());
    
    m_logged_in = true;
    m_password = "";
    
    parse = StringUtils::split_first(response);
    m_token = std::get<1>(parse);

    set_id(t_conn, t_req);
    
    return t_req.m_valid; 

fail:
    return false;
}

bool Accounts::set_username(std::string& t_username) {

    m_username = t_username;

    return true;
}

bool Accounts::set_password(std::string& t_password) {

    m_password = t_password;

    return true;
}

std::string Accounts::get_username(){
    return m_username;
}

bool Accounts::get_logged_in(){
    return m_logged_in;
}

int Accounts::get_id()
{
    return m_id;
}

std::string Accounts::get_token()
{
    return m_token;
}

/* Private */
bool Accounts::valid_response(Reply::Code t_code, std::string& t_res){

    std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if ( found != std::string::npos){
        return true;
    }

    return false;
}

int Accounts::set_id(ActiveConn& t_conn, Request& t_req)
{
    std::string response = "";
    std::string command = "SEARCH " + m_token + " " + m_username;

    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    if (false == t_req.m_valid)
    {
        LOG_ERR("Couldn't get user ID");
        return false;
    }
    
    response = TextData::to_string(t_req.data());
    if ( false == valid_response(Reply::r_201, response))
    {
        LOG_ERR("Response error to get user ID | %s", response.c_str());
        return false;
    }

    auto [user_id, rest] = StringUtils::split_first(response, ",");
    auto[key, pair] =  StringUtils::split_first(user_id, ":");

    try{
        m_id = std::stoi(pair);
    } catch(...){}
    

    return true;
}