#ifndef CLIENT_H
#define CLIENT_H

#include "contacts.hpp"
#include "accounts.hpp"
#include "request.hpp"
#include "job.hpp"
#include "active_conn.hpp"
#include "IO_strategy.hpp"
#include "text_io.hpp"
#include "chat.hpp"

#include <map>
#include <vector>

#define MAX_MSG_LEN 1024
#define SERVER_PORT 5000

class Client {

public:
    Client();
    ~Client();
    
    // Contacts commands
    static void contact_get_contacts        (Job &t_job);

    static void contact_list                (Job &t_job);
    static void contact_search              (Job &t_job);
    static void contact_add_user            (Job &t_job);
    static void contact_remove_user         (Job &t_job);
    static void contact_available           (Job &t_job);

    // User commands
    static void user_set_username           (Job &t_job);
    static void user_set_password           (Job &t_job);

    static void user_register_user          (Job &t_job);
    static void user_login                  (Job &t_job);

    static void user_get_username           (Job &t_job);
    static void user_get_logged_in          (Job &t_job);
    static void user_get_id                 (Job &t_job);

    // Chat commands
    static void chat_send                   (Job &t_job);
    static void chat_get_pending            (Job &t_job);
    static void chat_get_all                (Job &t_job);
    static void chat_deliver                (Job &t_job);

private:
    inline static Request       m_server_req = {};
    inline static ActiveConn    m_server_conn = ActiveConn(SERVER_PORT, new TextIO());
    
    inline static Accounts m_user;
    inline static Contacts m_contacts;
    inline static Chat     m_chat;

    static bool valid_response(Reply::Code t_code, std::string& t_res);
};

#endif
