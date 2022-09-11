#include "peer_to_peer.hpp"
#include "config.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "udp_text_io.hpp"

static Config *config = Config::get_instance();
const std::string P2P::m_DELIM = " ";

/* Constructors */

P2P::P2P(std::string &t_token) noexcept
    : m_token(t_token), m_inbounds(new UDPTextIO()),
      m_outbounds(new UDPTextIO()), m_status(Idle), m_last_reply(Reply::None) {

  bind_sockets();
}

/* Same constructor but moves the token string */
P2P::P2P(std::string &&t_token) noexcept
    : m_token(std::move(t_token)), m_inbounds(new UDPTextIO()),
      m_outbounds(new UDPTextIO()), m_status(Idle), m_last_reply(Reply::None) {

  bind_sockets();
}

/* Public */

P2P::Status P2P::status() const { return m_status; }
Reply::Code P2P::last_reply() const { return m_last_reply; }

/* */

void P2P::reset() {
  m_last_reply = Reply::None;
  m_peer_address = std::string();
  m_status = Idle;
}

/* */

void P2P::connect_peer(std::string &t_peer_id) {

  std::string response = send_server(ServerCommand::Connect, t_peer_id);

  if (m_last_reply == Reply::r_200) {
    m_status = Awaiting;

  } else if (m_last_reply == Reply::r_306) {
    m_status = Awaiting;
    LOG_ERR("%s", response.c_str());

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::accept_peer(std::string &t_peer_id) {

  std::string response = send_server(ServerCommand::Accept, t_peer_id);

  if (m_last_reply == Reply::r_200) {
    m_status = Accepted;
    m_peer_address = std::move(response);

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::reject_peer(std::string &t_peer_id) {

  std::string response = send_server(ServerCommand::Reject, t_peer_id);

  if (m_last_reply == Reply::r_200) {
    m_status = Idle;

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::ping_peer() {

  std::string response = send_server(ServerCommand::Ping);

  if (m_last_reply == Reply::r_201) {
    m_status = Accepted;
    m_peer_address = std::move(response);

  } else if (m_last_reply == Reply::r_203) {
    m_status = Awaiting;
    LOG_ERR("%s", response.c_str());

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::hangup_peer() {

  std::string response = send_server(ServerCommand::Hangup);

  if (m_last_reply == Reply::r_200) {
    m_status = Idle;

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* Private */

std::string P2P::send_server(ServerCommand::name t_cmd, std::string &t_arg) {

  std::string text_data =
      ServerCommand::to_string(t_cmd) + m_DELIM + m_token + m_DELIM + t_arg;

  return send_server(std::move(text_data));
}

std::string P2P::send_server(ServerCommand::name t_cmd) {

  std::string text_data = ServerCommand::to_string(t_cmd) + m_DELIM + m_token;

  return send_server(std::move(text_data));
}

std::string P2P::send_server(std::string &&t_text_data) {

  Request req = make_server_request(std::move(t_text_data));
  m_inbounds.respond(req);
  m_inbounds.receive(req);

  std::string response = TextData::to_string(req.data());
  auto [code, msg] = StringUtils::split_first(response);
  m_last_reply = Reply::from_string(code);

  return msg;
}

Request P2P::make_server_request(std::string &&t_text_data) {

  std::string address = config->get<const std::string>("SERVER_ADDRESS") + ":" +
                        config->get<const std::string>("UDP_PORT");

  Request req(std::move(address), true);
  req.set_data(new TextData(t_text_data));

  return req;
}

void P2P::bind_sockets() {
  m_inbounds.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
  m_outbounds.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
}
