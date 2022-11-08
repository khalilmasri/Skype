#include "contacts.hpp"
#include "logger.hpp"
#include "reply.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "fail_if.hpp"
#include "job.hpp"
#include "job_bus.hpp"

#include <string>
#include <sys/types.h>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>
#include <QString>
#include <QDebug>
#include <utility>

// /* Public */

bool Contacts::list(ActiveConn &t_conn, Request &t_req)
{

    std::string command = "LIST " + TextData::to_string(t_req.data());
    std::string response = "";
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);
    response = TextData::to_string(t_req.data());

    update_contacts(response);

    return true;

fail:
    return false;
}

bool Contacts::search(ActiveConn &t_conn, Request &t_req)
{

    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF(user == "");

    command = "SEARCH " + user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG(false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("Searched database and found user %s!", user.c_str());

    return true;

fail:
    return false;
}

bool Contacts::add_user(ActiveConn &t_conn, Request &t_req)
{

    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF(user == "");

    command = "ADD " + user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG(false == valid_response(Reply::r_200, response), response.c_str());

    LOG_INFO("Added user %s", user.c_str());

    list(t_conn, t_req);
    LOG_INFO("Added new user and updated contacts");

    return true;

fail:
    return false;
}

bool Contacts::remove_user(ActiveConn &t_conn, Request &t_req)
{

    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF(user == "");

    command = "REMOVE " + user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG(false == valid_response(Reply::r_200, response), response.c_str());

    list(t_conn, t_req);
    LOG_INFO("Removed user and updated contacts");

    return true;

fail:
    return false;
}

bool Contacts::available(ActiveConn &t_conn, Request &t_req)
{

    std::string response = "";
    std::string command = "";
    std::string user = TextData::to_string(t_req.data());
    FAIL_IF(user == "");

    command = "AVAILABLE " + user;
    t_req.set_data(new TextData(command));

    t_conn.respond(t_req);
    t_conn.receive(t_req);

    FAIL_IF(false == t_req.m_valid);

    response = TextData::to_string(t_req.data());

    FAIL_IF_MSG(false == valid_response(Reply::r_201, response), response.c_str());

    LOG_INFO("User %s is available!", user.c_str());

    return true;
fail:
    return false;
}

QHash<int, struct Details> Contacts::display_contacts()
{

    QHash<int, struct Details> contact_list;

    FAIL_IF_SILENT(true == m_online_contacts.empty());

    contact_list = m_online_contacts;

    return contact_list;

fail:
    return {};
}

// /* Private */

bool Contacts::valid_response(Reply::Code t_code, std::string &t_res)
{

    std::string code = Reply::get_message(t_code);
    auto found = t_res.find(code);

    if (found != std::string::npos)
    {
        return true;
    }

    return false;
}

void Contacts::update_contacts(std::string t_response)
{

    StringUtils::StringVector users = StringUtils::split(t_response);

    auto old_contacts = m_online_contacts;
    m_online_contacts.clear();

    for (auto &user : users)
    {
        pair_contact_details(user);
    }

    if (old_contacts.size() != m_online_contacts.size() || m_online_contacts.empty())
    {
        LOG_DEBUG("Updating map");
        JobBus::create({Job::DISP_CONTACTS});
        return;
    }

    for (auto &contact : m_online_contacts)
    {
        if (false == old_contacts.contains(contact.ID))
        {
            LOG_DEBUG("Updating map");
            JobBus::create({Job::DISP_CONTACTS});
            return;
        }
        else if (old_contacts[contact.ID].online != contact.online)
        {
            LOG_DEBUG("Updating map");
            JobBus::create({Job::DISP_CONTACTS});
            return;
        }
        else if (old_contacts[contact.ID].awaiting != contact.awaiting)
        {
            LOG_DEBUG("Sending Awaitng call");
            Job job = {Job::AWAITING};
            job.m_intValue = contact.ID;
            job.m_boolValue = contact.awaiting;
            JobBus::create(job);
            return;
        }
    }
}

void Contacts::pair_contact_details(std::string t_user)
{

    StringUtils::StringVector user_fields = StringUtils::split(t_user, ",");

    // std::cout << t_user << std::endl;

    Details details;
    int ID = 0;

    for (auto field : user_fields)
    {

        auto [key, pair] = StringUtils::split_first(field, ":");

        if (key == "id")
        {
            try
            {
                ID = stoi(pair);
                details.ID = ID;
            }
            catch (...)
            {
                LOG_ERR("Couldn't get ID")
            };
        }

        if (key == "username")
        {
            details.username = QString::fromStdString(pair);
        }

        if (key == "online")
        {
            if (pair == "true")
            {
                details.online = true;
            }
            else
            {
                details.online = false;
            }
        }

        if (key == "awaiting")
        {
            if (pair == "true")
            {
                details.awaiting = true;
            }
            else
            {
                details.awaiting = false;
            }
        }
    }

    if (ID != 0)
    {
        m_online_contacts.insert(ID, details);
        LOG_TRACE("User => %s, Online => %s Awaiting => %s", details.username.toLocal8Bit().data(), (details.online) ? "TRUE" : "FALSE", (details.awaiting) ? "TRUE" : "FALSE");
    }
}
