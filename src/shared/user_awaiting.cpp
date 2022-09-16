#include "user_awaiting.hpp"
#include "logger.hpp"
#include "string_utils.hpp"

/* Constructors AwaitingUser */
AwaitingUser::AwaitingUser(int t_id, int t_peer_id,
                           const std::string &t_address)
    : m_id(t_id), m_peer_id(t_peer_id), m_address(t_address) {}

AwaitingUser::AwaitingUser(int t_id, int t_peer_id,
                           const std::string &t_address,
                           const std::string &t_local_address)
    : m_id(t_id), m_peer_id(t_peer_id), m_address(t_address),
      m_local_address(t_local_address) {

  /* Appends port from public to the local address */
  auto [_, port] = StringUtils::split_first(m_address, ":");
  m_local_address += ":" + port;
}

/* Getters */

int AwaitingUser::id() const { return m_id; }
int AwaitingUser::peer_id() const { return m_peer_id; }

/* if user and peer are on the same network return local address */
std::string AwaitingUser::address() const {

  if (has_same_address()) {
    return m_local_address;
  }

  return m_address;
};

/* */

std::string AwaitingUser::peer_address() const {

/* returns local address when in the same network */
  if (has_same_address()) {
    return m_peer_local_address;
  }

  return m_peer_address;
}

/* */

std::string AwaitingUser::address_type(){
  if (has_same_address()) {
    return "LOCAL";
  }

  return "WEB";
}

/* Setters */

void AwaitingUser::set_peer_address(const std::string &t_peer_address) {
  m_peer_address = t_peer_address;
}

void AwaitingUser::set_peer_local_address(
    const std::string &t_peer_local_address) {
  /* take the port from public address */
  auto [_, port] = StringUtils::split_first(m_peer_address, ":");
  m_peer_local_address = t_peer_local_address + ":" + port;
}

/* Private */

bool AwaitingUser::has_same_address() const {
  auto[user_address, _uport] = StringUtils::split_first(m_address, ":");
  auto[peer_address, _pport] = StringUtils::split_first(m_address, ":");

  return user_address == peer_address;
}

/** AwaitingUsers **/

bool AwaitingUsers::insert(AwaitingUser &&t_awaiting_user) noexcept {
  // returns a pair with iterator and bool if insert was successful.
  auto result =
      m_awaiting_users.insert({t_awaiting_user.id(), t_awaiting_user});

  if (!result.second) {
    LOG_ERR("Did not insert user id '%d' to AwaitingUsers because it already "
            "exists.",
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
    LOG_ERR("Could not delete AwaitingUser id '%d' because it does not exist.",
            t_awaiting_user_id);

    return false;
  }

  m_awaiting_users.erase(t_awaiting_user_id);
  return true;
}

bool AwaitingUsers::exists(int t_awaiting_user_id) const noexcept {
  return m_awaiting_users.find(t_awaiting_user_id) != m_awaiting_users.end();
}
