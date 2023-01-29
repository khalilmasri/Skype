#include "peer_to_peer.hpp"
#include "config.hpp"
#include "doctest.h"
#include "stream_io.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "udp_text_io.hpp"
#include <thread>

static Config *config = Config::get_instance();

/* Constructors */

P2P::P2P(std::string &t_token)
    : m_token(t_token), m_conn(new UDPTextIO()), m_status(Idle), m_type(None),
      m_last_reply(Reply::None), m_local_ip(), m_network_type(Unselected) {

  bind_sockets();
}

/* Same constructor but moves the token string */
P2P::P2P(std::string &&t_token)
    : m_token(std::move(t_token)), m_conn(new UDPTextIO()), m_status(Idle),
      m_type(None), m_last_reply(Reply::None), m_local_ip(),
      m_network_type(Unselected) {

  bind_sockets();
}

/* Public */

auto P2P::status() const -> P2P::Status { return m_status; }
auto P2P::type() const -> P2P::Type { return m_type; }
auto P2P::last_reply() const -> Reply::Code { return m_last_reply; }
auto P2P::make_request() const -> Request {
  return Request(m_peer_address, true);
}

/* */

auto P2P::status_to_string() const -> std::string {

  switch (m_status) {
  case Idle:
    return "Idle";

  case Awaiting:
    return "Awaiting";

  case Accepted:
    return "Accepted";

  case Connected:
    return "Connected";

  case Error:
    return "Error";
  }
}

/* */

auto P2P::type_to_string() const -> std::string {

  switch (m_type) {
  case Initiator:
    return "Initiator";

  case Acceptor:
    return "Acceptor";

  case None:
    return "None";
  }
}

/* Sending Data to peer */

void P2P::send_package(Request &t_req) {

  if (m_status == Connected) {
    m_conn.respond(t_req);

  } else {
    t_req.m_valid = false;
    LOG_DEBUG("Cannot send package. P2P Not connected.");
  }
}

/* Receiving data from peer */

void P2P::receive_package(Request &t_req) {

  if (m_status == Connected) {
    m_conn.receive(t_req);
  } else {
    t_req.m_valid = false;
    LOG_DEBUG("Cannot receive package. P2P Not connected.");
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

  Request req(m_peer_address + ":" + std::to_string(m_conn.get_port()),
              true); // m_valid = true

  /* if m_network_type = WEB handshake will hole punch */
  if (m_type == Acceptor) {
    handshake_acceptor(req, m_network_type);
  }

  if (m_type == Initiator) {
    handshake_initiator(req, m_network_type);
  }
  // when connected swap the IO type to data stream
  if (m_status == Connected) {
    LOG_INFO("Setting UDP connection strategy to StreamIO");
    m_conn.set_strategy(new StreamIO());
  };
};

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
    m_network_type = address_type == m_LOCAL ? Local : Web;

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

  if (m_status == Accepted) {
    LOG_TRACE("This connection has already been accepted. Ignoring ping...");
    return;
  }

  std::string response = send_server(ServerCommand::Ping);

  if (m_last_reply == Reply::r_201) {

    m_status = Accepted;

    auto [address, address_type] = StringUtils::split_first(response);
    LOG_INFO("successful ping from address `%s` type `%s`.", address.c_str(),
             address_type.c_str());

    m_peer_address = std::move(address);
    m_network_type = address_type == "LOCAL" ? Local : Web;
    return;

  } else if (m_last_reply == Reply::r_203) {
    // if cannot find the ping try a couple more times or ignore.
    m_status = Awaiting;
    LOG_INFO("%s", response.c_str());

  } else {
    m_status = Error;
    LOG_ERR("The response was '%s' and should have been 201 or 203",
            response.c_str());
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

auto P2P::send_server(ServerCommand::name t_cmd, std::string &t_arg)
    -> std::string {

  std::string text_data =
      ServerCommand::to_string(t_cmd) + m_DELIM + m_token + m_DELIM + t_arg;

  return send_server(std::move(text_data));
}

/* */

auto P2P::send_server(ServerCommand::name t_cmd) -> std::string {

  std::string text_data = ServerCommand::to_string(t_cmd) + m_DELIM + m_token;

  return send_server(std::move(text_data));
}

/* */

auto P2P::send_server(std::string &&t_text_data) -> std::string {

  Request req = make_server_request(std::move(t_text_data));
  m_conn.respond(req);
  m_conn.receive(req);

  std::string response = TextData::to_string(req.data());
  auto [code, msg] = StringUtils::split_first(response);
  m_last_reply = Reply::from_string(code);

  return msg;
}

void P2P::handshake_acceptor(Request &t_req, PeerNetwork t_peer_network) {

  /* peers in local network does not require to hole punch the NAT */
  if (t_peer_network == Web) {
    hole_punch(t_req);
  }

  LOG_DEBUG("Acceptor: waiting for handshake confirmation from '%s'. ",
            t_req.m_address.c_str());

  bool got_res = m_conn.receive(t_req);

  // acceptor will get trying to connect until it gets a response
  retry(t_req, got_res);

  if (m_status == Error) {
    return;
  }

  std::string response = TextData::to_string(t_req.data());

  LOG_DEBUG("Acceptor: got response '%s' ", response.c_str());

  std::string ok_msg = Reply::get_message(Reply::r_200);

  if (response == ok_msg) {
    LOG_INFO("Acceptor: P2P handshake with '%s' was sucessful. ",
             t_req.m_address.c_str());

    m_status = Connected;
    t_req.set_data(new TextData(ok_msg));
    m_conn.respond(t_req);

  } else {
    LOG_ERR("P2P handshake message '%s' should be '200 OK'. Handshake failed.",
            response.c_str());

    m_status = Error;
  }
}

/* */

void P2P::handshake_initiator(Request &t_req, PeerNetwork t_peer_network) {

  /* peers in local network does not require to hole punch the NAT */
  if (t_peer_network == Web) {
    hole_punch(t_req);
  }

  std::string ok_msg = Reply::get_message(Reply::r_200);
  t_req.set_data(new TextData(ok_msg));

  LOG_DEBUG("Intiator: Sending 200 OK to confirm.");
  m_conn.respond(t_req);

  LOG_DEBUG("Intiator: Handshake sent. now waiting for OK response...")
  bool got_res = m_conn.receive(t_req);

  // initiator will get trying to connect until it gets a response
  retry(t_req, got_res);

  if (m_status == Error) {
    return;
  }

  std::string response = TextData::to_string(t_req.data());

  if (ok_msg == response) {
    LOG_INFO("P2P handshake with '%s' was sucessful. ",
             t_req.m_address.c_str());
    m_status = Connected;
  } else {
    LOG_ERR("Initiator: P2P handshake message '%s' should be '%s'. "
            "Handshake failed.",
            response.c_str(), ok_msg.c_str());
    m_status = Error;
  }
}

/* */

auto P2P::invalid_to_handshake() -> bool {

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

void P2P::bind_sockets() {
  m_conn.bind_socket(config->get<const std::string>("SERVER_ADDRESS"));
}

/* */

auto P2P::make_server_request(std::string &&t_text_data) -> Request {

  std::string address = config->get<const std::string>("SERVER_ADDRESS") + ":" +
                        config->get<const std::string>("UDP_PORT");

  Request req(std::move(address), true);
  req.set_data(new TextData(t_text_data));

  return req;
}

/* */

void P2P::retry(Request &t_req, bool got_res) {

  if (got_res) {
    return;
  }

  bool should_retry = !got_res;
  std::size_t nb_trials = 0;

  while (should_retry && nb_trials < m_MAX_PUNCH_TRIALS) {
    LOG_DEBUG(
        "%s: Did not receive response peer. Sending punch message again...",
        type_to_string().c_str())

    hole_punch(t_req);

    /* receive is non-blocking here. It will timeout after
     * UDPConn::m_RECEIVE_TIMEOUT  is reached It returns false in case of errors
     * or if nothing has been received
     * */

    bool success = m_conn.receive(t_req);

    if (success) {
      LOG_DEBUG("Hole punch retrials was successful.")
      return;
    }

    should_retry = !success;
    nb_trials++;
  }

  LOG_DEBUG("Giving up on hole punch after %lu trials.", nb_trials);
  m_status = Error;
}

/* */

void P2P::hole_punch(Request &t_req) {

  std::string ok_msg = Reply::get_message(Reply::r_200);

  LOG_DEBUG("Hole punch: Sending 200 OK to '%s'...", t_req.m_address.c_str());

  t_req.set_data(new TextData(ok_msg));
  m_conn.respond(t_req);
}
