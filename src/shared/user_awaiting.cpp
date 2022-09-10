#include "user_awaiting.hpp"
#include "logger.hpp"

AwaitingUser::AwaitingUser(int t_id, int t_peer_id, const std::string &t_address)
    : m_id(t_id), m_peer_id(t_peer_id), m_address(t_address) {}

int AwaitingUser::id() const { return m_id; }
int AwaitingUser::peer_id() const { return m_peer_id; }
std::string AwaitingUser::address() const { return m_address; };
std::string AwaitingUser::peer_address() const { return m_peer_address; }
void AwaitingUser::set_peer_address(const std::string &t_peer_address) { m_peer_address = t_peer_address; }

/** AwaitingUsers **/

bool AwaitingUsers::insert(AwaitingUser &&t_awaiting_user) noexcept {
  // returns a pair with iterator and bool if insert was succesful.
  auto result =
      m_awaiting_users.insert({t_awaiting_user.id(), t_awaiting_user});

  if (!result.second) {
    LOG_ERR(
        "Did not insert user id %d to AwaitingUsers because it already exists.",
        t_awaiting_user.id());
  }

  return result.second;
}

// This method throws and must be wrapped in a try catch.
AwaitingUser &AwaitingUsers::get(int t_awaiting_user_id) {
  return m_awaiting_users.at(t_awaiting_user_id);
}

bool AwaitingUsers::destroy(int t_awaiting_user_id) noexcept {

  // check if exists before destroying
  if (m_awaiting_users.find(t_awaiting_user_id) == m_awaiting_users.end()) {
    LOG_ERR("Could not delete AwaitingUser id %d because it does not exist.",
            t_awaiting_user_id);
    return false;
  }

  m_awaiting_users.erase(t_awaiting_user_id);

  return true;
}

bool AwaitingUsers::exists(int t_awaiting_user_id) const noexcept {
  return m_awaiting_users.find(t_awaiting_user_id) == m_awaiting_users.end();
}
