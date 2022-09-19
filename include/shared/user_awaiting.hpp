#ifndef USER_AWAITING_H
#define USER_AWAITING_H

#include <string>
#include <unordered_map>

struct AwaitingUser {

  AwaitingUser(int t_id, int t_peer_id, const std::string &t_address);
  AwaitingUser(int t_id, int t_peer_id, const std::string &t_address, const std::string &t_local_address);

    /* Getters */
    int         id() const;
    int         peer_id() const;
    std::string address() const;
    std::string peer_address() const;

    /* Setters */
    void        set_peer_local_address(const std::string &t_peer_local_address);
    void        set_peer_address(const std::string &t_peer_address);

    /* return m_LOCAL or m_WEB */
    std::string address_type();

    private:
    int         m_id;
    int         m_peer_id;

    std::string m_address;
    std::string m_local_address;

    std::string m_peer_address;
    std::string m_peer_local_address;

    bool has_same_address() const;

    /* constants */
    static const std::string m_WEB;
    static const std::string m_LOCAL;
};

class AwaitingUsers {
  public: 

    bool         insert(AwaitingUser &&t_awaiting_user) noexcept; 
    AwaitingUser &get(int t_awaiting_user_id); 
    bool         destroy(int t_awaiting_user_id) noexcept;
    bool         exists(int t_awaiting_user_id) const noexcept;

  private:
  typedef std::unordered_map<int, AwaitingUser> AwaitingUsersMap;

  AwaitingUsersMap m_awaiting_users;
};

#endif
