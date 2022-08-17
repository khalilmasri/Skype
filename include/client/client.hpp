#ifndef CLIENT_H
#define CLIENT_H

#include "contacts.hpp"
#include "request.hpp"
#include "job.hpp"
#include "user.hpp"
#include "active_conn.hpp"
#include "IO_strategy.hpp"
#include "text_io.hpp"

#include <map>
#include <vector>

#define MAX_MSG_LEN 1024
#define SERVER_PORT 5000

class Client {

public:
    Client();
    Client(int t_port){return;}; // Delete after we fix the client with GUI
    ~Client();
    
    // Contacts commands
    static void contact_get_current_contact (Job &t_job);
    static void contact_set_current_contact (Job &t_job);
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

private:
    inline static ActiveConn server_conn = ActiveConn(SERVER_PORT, new TextIO());
    inline static Request server_req = {};
    
    inline static User m_user;
    inline static Contacts m_contacts;

    static bool valid_response(Reply::Code t_code, std::string& t_res);
};

#endif
