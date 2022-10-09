#include "user_awaiting.hpp"
#include "logger.hpp"
#include "string_utils.hpp"

/* Constants */
const std::string AwaitingUser::m_WEB = "WEB";
const std::string AwaitingUser::m_LOCAL = "LOCAL";

/* Constructors AwaitingUser */
AwaitingUser::AwaitingUser(Ids t_ids, std::string t_address)
    : m_id(t_ids.user_id), m_peer_id(t_ids.peer_id),
      m_address(std::move(t_address)) {}

AwaitingUser::AwaitingUser(Ids t_ids, std::string t_address,
                           std::string t_local_address)

    : m_id(t_ids.user_id), m_peer_id(t_ids.user_id),
      m_address(std::move(t_address)),
      m_local_address(std::move(t_local_address)) {

  /* Appends port from public to the local address */
  auto [_, port] = StringUtils::split_first(m_address, ":");
  m_local_address += ":" + port;
}

/* Getters */

auto AwaitingUser::id() const -> int { return m_id; }
auto AwaitingUser::peer_id() const -> int { return m_peer_id; }

auto AwaitingUser::address() const -> std::string {

  /* returns local address when in the same network */
  if (has_same_address()) {
    return m_local_address;
  }

  return m_address;
};

/* */

auto AwaitingUser::peer_address() const -> std::string {

  if (has_same_address()) {
    return m_peer_local_address;
  }

  return m_peer_address;
}

/* */

auto AwaitingUser::address_type() -> std::string {
  if (has_same_address()) {
    return m_LOCAL;
  }

  return m_WEB;
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

auto AwaitingUser::has_same_address() const -> bool {
  auto [user_address, _uport] = StringUtils::split_first(m_address, ":");
  auto [peer_address, _pport] = StringUtils::split_first(m_peer_address, ":");

  return user_address == peer_address;
}

/** AwaitingUsers **/

auto AwaitingUsers::insert(AwaitingUser &&t_awaiting_user) noexcept -> bool {
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

// IMPORTANT: This method throws and must be wrapped in a try catch.
auto AwaitingUsers::get(int t_awaiting_user_id) -> AwaitingUser & {
  return m_awaiting_users.at(t_awaiting_user_id);
}

auto AwaitingUsers::destroy(int t_awaiting_user_id) noexcept -> bool {

  // check if exists before destroying
  if (m_awaiting_users.find(t_awaiting_user_id) == m_awaiting_users.end()) {
    LOG_ERR("Could not delete AwaitingUser id '%d' because it does not exist.",
            t_awaiting_user_id);

    return false;
  }

  m_awaiting_users.erase(t_awaiting_user_id);
  return true;
}

auto AwaitingUsers::exists(int t_awaiting_user_id) const noexcept -> bool {
  return m_awaiting_users.find(t_awaiting_user_id) != m_awaiting_users.end();
}
