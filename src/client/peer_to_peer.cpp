#include "peer_to_peer.hpp"
#include "config.hpp"
#include "doctest.h"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "udp_text_io.hpp"

static Config *config = Config::get_instance();
const std::string P2P::m_DELIM = " ";

// TODO: Server now takes a the local IP when public is the same.
//       now must create the UDP connection for the local network.

/* Constructors */

P2P::P2P(std::string &t_token) noexcept
    : m_token(t_token), m_inbounds(new UDPTextIO()),
      m_outbounds(new UDPTextIO()), m_status(Idle), m_type(None),
      m_last_reply(Reply::None), m_network_type(Unselected) {

  bind_sockets();
}

/* Same constructor but moves the token string */
P2P::P2P(std::string &&t_token) noexcept
    : m_token(std::move(t_token)), m_inbounds(new UDPTextIO()),
      m_outbounds(new UDPTextIO()), m_status(Idle), m_type(None),
      m_last_reply(Reply::None), m_network_type(Unselected) {

  bind_sockets();
}

/* Public */

P2P::Status P2P::status() const { return m_status; }
P2P::Type P2P::type() const { return m_type; }
Reply::Code P2P::last_reply() const { return m_last_reply; }

/* */

std::string P2P::status_to_string() const {

  switch (m_status) {
  case Idle:
    return std::string("Idle");

  case Awaiting:
    return std::string("Awaiting");

  case Accepted:
    return std::string("Accepted");

  case Connected:
    return std::string("Connected");

  case Error:
    return std::string("Error");
  }
}

/* */

std::string P2P::type_to_string() const {

  switch (m_type) {
  case Initiator:
    return std::string("Initiator");

  case Acceptor:
    return std::string("Acceptor");

  case None:
    return std::string("None");
  }
}

/* */

void P2P::reset() {
  m_last_reply = Reply::None;
  m_peer_address = {};
  m_status = Idle;
  m_type = None;
}

/* */

void P2P::handshake_peer() {

  if (invalid_to_handshake()) {
    return;
  }

  Request req(m_peer_address, true);

  /* if m_network_type = WEB handshake will hole punch */
  if (m_type == Acceptor) {
    handshake_acceptor(req, m_network_type);
  }

  if (m_type == Initiator) {
    handshake_initiator(req, m_network_type);
  }
};

void P2P::stream_out() {

  if (m_status != Connected) {
    LOG_ERR("P2P::Status must be 'Connected' to stream. Was '%s'",
            status_to_string().c_str());
    return;
  }

  Request req(m_peer_address, true);

  while (true) {
    req.set_data(new TextData("stream!"));
    m_inbounds.respond(req);
  }
}

void P2P::stream_in() {

  if (m_status != Connected) {
    LOG_ERR("P2P::Status must be 'Connected' to stream. Was '%s'",
            status_to_string().c_str());
    return;
  }

  Request req(m_peer_address, true);

  while (true) {
    m_inbounds.receive(req);
    std::string received = TextData::to_string(req.data());
  }
}

/* */

void P2P::connect_peer(std::string &t_peer_id) {

  /* add client local addr as arg to CONNECT */
  std::string argument = t_peer_id + " " + m_local_ip.get_first();

  std::string response = send_server(ServerCommand::Connect, argument);

  m_type = Initiator;

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

  m_type = Acceptor;

  /* add client local addr as arg to ACCEPT */
  std::string argument = t_peer_id + " " + m_local_ip.get_first();
  std::string response = send_server(ServerCommand::Accept, argument);

  if (m_last_reply == Reply::r_201) {
    m_status = Accepted;
    auto [address, address_type] = StringUtils::split_first(response);
    m_peer_address = std::move(address);
    m_network_type = address_type == "LOCAL" ? Local : Web;

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::reject_peer(std::string &t_peer_id) {

  m_type = Acceptor;

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

  if (m_type != Initiator) {
    LOG_ERR("A peer cannot PING if he has not initiated with CONNECT. P2P:Type "
            "was '%s'.",
            type_to_string().c_str());
    return;
  }

  std::string response = send_server(ServerCommand::Ping);

  if (m_last_reply == Reply::r_201) {
    m_status = Accepted;
    auto [address, address_type] = StringUtils::split_first(response);
    m_peer_address = std::move(address);
    m_network_type = address_type == "LOCAL" ? Local : Web;

  } else if (m_last_reply == Reply::r_203) {
    m_status = Awaiting;
    LOG_INFO("%s", response.c_str());

  } else {
    m_status = Error;
    LOG_ERR("%s", response.c_str());
  }
}

/* */

void P2P::hangup_peer() {

  if (m_type != Initiator) {
    LOG_ERR("A peer cannot HANGUP if he has not initiated with CONNECT. "
            "P2P:Type was '%s'.",
            type_to_string().c_str());
    return;
  }

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

/* */

std::string P2P::send_server(ServerCommand::name t_cmd) {

  std::string text_data = ServerCommand::to_string(t_cmd) + m_DELIM + m_token;

  return send_server(std::move(text_data));
}

/* */

std::string P2P::send_server(std::string &&t_text_data) {

  Request req = make_server_request(std::move(t_text_data));
  m_inbounds.respond(req);
  m_inbounds.receive(req);

  std::string response = TextData::to_string(req.data());
  auto [code, msg] = StringUtils::split_first(response);
  m_last_reply = Reply::from_string(code);

  return msg;
}

void P2P::handshake_acceptor(Request &t_req, PeerNetwork t_peer_network) {

  /* peers in local network does not require to whole punch NAT */
  if (t_peer_network == Web) {
    hole_punch(t_req);
  }

  LOG_DEBUG("Acceptor: waiting for handshake confirmation from '%s'. ",
            t_req.m_address.c_str());

  std::string ok = Reply::get_message(Reply::r_200);

  m_inbounds.receive(t_req);
  std::string response = TextData::to_string(t_req.data());

  LOG_DEBUG("Acceptor: got response '%s' ", response.c_str());

  if (response == ok) {
    LOG_INFO("Acceptor: P2P handshake with '%s' was sucessful. ",
             t_req.m_address.c_str());

    m_status = Connected;
    t_req.set_data(new TextData(ok));
    m_inbounds.respond(t_req);

  } else {
    LOG_ERR("P2P handshake message '%s' should be '200 OK'. Handshake failed.",
            response.c_str());

    m_status = Error;
  }
}

/* */

void P2P::handshake_initiator(Request &t_req, PeerNetwork t_peer_network) {

  /* peers in local network does not require to whole punch NAT */
  if (t_peer_network == Web) {
    hole_punch(t_req);
  }

  std::string ok = Reply::get_message(Reply::r_200);
  t_req.set_data(new TextData(ok));

  LOG_DEBUG("Intiator: Sending 200 OK to confirm.");
  m_inbounds.respond(t_req);

  LOG_DEBUG("Intiator: Handhake sent. now waiting for 0K response...")
  m_inbounds.receive(t_req);

  std::string response = TextData::to_string(t_req.data());

  if (response == ok) {
    LOG_INFO("P2P handshake with '%s' was sucessful. ",
             t_req.m_address.c_str());
    m_status = Connected;
  } else {
    LOG_ERR("Initiator: P2P handshake message '%s' should be '200 OK'. "
            "Handshake failed.",
            response.c_str());
    m_status = Error;
  }
}

/* */

bool P2P::invalid_to_handshake() {

  if (m_status != Accepted) {
    LOG_ERR("P2P::Status must be 'Accepted' to handshake. Was '%s'",
            status_to_string().c_str());
    return true;
  }

  if (m_peer_address.empty()) {
    LOG_ERR("'%s' failed to handshake. Peer address was not set correctly.",
            type_to_string().c_str());
    return true;
  }

  if (m_network_type == Unselected) {
    LOG_ERR("P2P::m_network_type must be Web or Local to handshake.");
    return true;
  }

  return false;
}

/* */

Request P2P::make_server_request(std::string &&t_text_data) {

  std::string address = config->get<const std::string>("SERVER_ADDRESS") + ":" +
                        config->get<const std::string>("UDP_PORT");

  Request req(std::move(address), true);
  req.set_data(new TextData(t_text_data));

  return req;
}

/* */

void P2P::bind_sockets() {
  m_inbounds.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
  m_outbounds.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
}

/* */

void P2P::hole_punch(Request &t_req) {

  std::string ok = Reply::get_message(Reply::r_200);

  LOG_DEBUG("Hole punch: Sending 200 OK to '%s'...", t_req.m_address.c_str());

  t_req.set_data(new TextData(ok));
  m_inbounds.respond(t_req);
}
