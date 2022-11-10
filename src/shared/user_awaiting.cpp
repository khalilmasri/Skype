#include "user_awaiting.hpp"
#include "logger.hpp"
#include "string_utils.hpp"
#include <chrono>

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

AwaitingUsers::AwaitingUsers() {

  // spawn a thread that checks call requests go adrift/detached.
  auto worker = [this]() { timeout_loop(); };
  m_timeout_worker = std::thread(worker);
  m_timeout_worker.detach();
}

AwaitingUsers::~AwaitingUsers() { m_exit = true; }

/* */

auto AwaitingUsers::insert(AwaitingUser &&t_awaiting_user) noexcept -> bool {
  // returns a pair with iterator and bool if insert was successful.
  auto result =
      m_awaiting_users.insert({t_awaiting_user.id(), t_awaiting_user});

  if (!result.second) {
    LOG_ERR("Did not insert user id '%d' to AwaitingUsers because it already "
            "exists.",
            t_awaiting_user.id());
  }

  // add to timeouts
  if (result.second) {
    m_timeouts.emplace_back(t_awaiting_user.id(), Created);
  }

  return result.second;
}

// IMPORTANT: This method throws and must be wrapped in a try catch.
auto AwaitingUsers::get(int t_awaiting_user_id) -> AwaitingUser & {
  return m_awaiting_users.at(t_awaiting_user_id);
}

/* */

auto AwaitingUsers::destroy(int t_awaiting_user_id) noexcept -> bool {

  // check if exists before destroying

  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_awaiting_users.find(t_awaiting_user_id) == m_awaiting_users.end()) {
    LOG_ERR("Could not delete AwaitingUser id '%d' because it does not exist.",
            t_awaiting_user_id);

    return false;
  }

  try {
  m_awaiting_users.erase(t_awaiting_user_id);
  } catch(...){
    LOG_ERR("Could not delete AwaitingUser id '%d' because it does not exist.",
            t_awaiting_user_id);

   return false;
  }
  return true;
}

auto AwaitingUsers::exists(int t_awaiting_user_id) const noexcept -> bool {
  return m_awaiting_users.find(t_awaiting_user_id) != m_awaiting_users.end();
}

/* */

void AwaitingUsers::ping(int t_awaiting_user_id) noexcept {
  update_timeout_status(t_awaiting_user_id, Pinged);
}

/* Private */

void AwaitingUsers::update_timeout_status(int t_awaiting_user, TimeoutStatus t_new_status) {

  for (auto &[user_id, timeout_status] : m_timeouts) {

    if (user_id == t_awaiting_user) {

      std::lock_guard<std::mutex> lock(m_mutex);
      timeout_status = t_new_status;
    }
  }
}

/* */

void AwaitingUsers::timeout_loop() {

  // this worker will run until parent class is destroyed.
  while (!m_exit) {

    if (!m_timeouts.empty()) {
      int index = 0;
      for (auto &[user_id, timeout_status] : m_timeouts) {

        if (timeout_status == Detached) {
          LOG_INFO("The Timeout worker has destroyed the awaiting_user '%d' because it became detached.", user_id);
          destroy(user_id);
          m_timeouts.erase(m_timeouts.begin() + index);

        } else {
          std::string status = timeout_status == Pinged ? "Pinged" : "Created";
          LOG_INFO("Timeout worker set awaiting_user id '%d' from '%s' to 'Detached'.", user_id, status.c_str());
          update_timeout_status(user_id, Detached);

        }
      }
    }

    std::this_thread::sleep_for(
        std::chrono::seconds(m_TIMEOUT_WORKER_FREQUENCY));
  };
}
