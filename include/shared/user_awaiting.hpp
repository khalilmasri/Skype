#ifndef USER_AWAITING_H
#define USER_AWAITING_H

#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <functional>


struct AwaitingUser {

  struct Ids { int user_id; int peer_id; };

  AwaitingUser(Ids ids, std::string t_address);
  AwaitingUser(Ids ids, std::string t_address, std::string t_local_address);

    /* Getters */
    [[nodiscard]] auto id() const -> int;
    [[nodiscard]] auto peer_id() const -> int;
    [[nodiscard]] auto address() const -> std::string;
    [[nodiscard]] auto peer_address() const -> std::string;

    /* Setters */
    void               set_peer_local_address(const std::string &t_peer_local_address);
    void               set_peer_address(const std::string &t_peer_address);

    /* return m_LOCAL or m_WEB */
    auto               address_type() -> std::string;

    private:
    int         m_id;
    int         m_peer_id;

    std::string m_address;
    std::string m_local_address;

    std::string m_peer_address;
    std::string m_peer_local_address;

    [[nodiscard]] auto has_same_address() const -> bool;

    /* constants */
    static const std::string m_WEB;
    static const std::string m_LOCAL;
};

class AwaitingUsers {
  public: 

     AwaitingUsers();
     ~AwaitingUsers();

    auto insert(AwaitingUser &&t_awaiting_user) noexcept -> bool; 
    auto get(int t_awaiting_user_id) -> AwaitingUser &; 
    auto destroy(int t_awaiting_user_id) noexcept -> bool;
    auto exists(int t_awaiting_user_id) const noexcept -> bool;

    void ping(int t_awaiting_user_id) noexcept;

  private:
  enum TimeoutStatus     {Created, Pinged, Detached };
  using AwaitingUsersMap = std::unordered_map<int, AwaitingUser>;
  using UserId           = int;
  using Timeout          = std::tuple<UserId, TimeoutStatus>;

  AwaitingUsersMap m_awaiting_users;
  std::vector<Timeout> m_timeouts;

  std::mutex m_mutex;
  std::thread m_timeout_worker;
  bool        m_exit = false;
  inline static const int m_TIMEOUT_WORKER_FREQUENCY = 2;

  void update_timeout_status(int t_awaiting_user, TimeoutStatus t_new_status);

  /* thread worker will run indefinetely */
  void timeout_loop();
};

#endif
