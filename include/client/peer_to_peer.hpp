#ifndef PEER_TO_PEER_H
#define PEER_TO_PEER_H
#include "udp_conn.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "local_ip.hpp"
#include "server_commands.hpp"
#include <functional>

class P2P {

  /**** PUBLIC INTERFACE ***/

  public:
  enum Status {Idle, Awaiting, Accepted, Connected, Error};
  enum Type   { Initiator, Acceptor, None };

  explicit P2P(std::string &t_token) ;
  explicit P2P(std::string &&t_token) ;

  /* getters */
  [[nodiscard]] auto status() const -> Status;
  [[nodiscard]] auto status_to_string() const -> std::string;
  [[nodiscard]] auto type() const -> Type;
  [[nodiscard]] auto type_to_string() const -> std::string;
  [[nodiscard]] auto last_reply() const -> Reply::Code;

/* this method will return an read to go Request with the peers address for connection */
  [[nodiscard]] auto make_request() const -> Request;

  /* UDP connection to exchange data */
  void          send_package(Request &t_req);
  void          receive_package(Request &t_req);

  /* Reset the P2P connection */
  void          reset();

  /* Peers are ready to connect to each other */
  void          handshake_peer();

  /* Calls to the server to connect to another peer */
  void          connect_peer(std::string &t_peer_id);
  void          accept_peer(std::string &t_peer_id);
  void          reject_peer(std::string &t_peer_id);
  void          ping_peer();
  void          hangup_peer();

  /**** PRIVATE IMPLEMENTATION ***/

  private:
  /* Defines if peers are in the local network or the Web */
  enum PeerNetwork {Local, Web, Unselected};

  /* Defines the direction of the stream */
  enum StreamDir {In, Out};

  std::string   m_peer_address;
  std::string   m_token;
  UDPConn       m_conn;
  Status        m_status;
  Type          m_type;
  Reply::Code   m_last_reply;
  LocalIP       m_local_ip; /* Abstraction to grab clients local IP */
  PeerNetwork   m_network_type;

  /* return the res message from the Server and sets m_last_reply with the Reply::Code */
  auto          send_server(ServerCommand::name t_cmd, std::string &t_arg) -> std::string;
  auto          send_server(ServerCommand::name t_cmd) -> std::string;
  auto          send_server(std::string &&t_text_data) -> std::string;

  void          handshake_acceptor(Request &t_req, PeerNetwork t_peer_network);
  void          handshake_initiator(Request &t_req , PeerNetwork t_peer_network);

  auto          invalid_to_handshake() -> bool;
  void          bind_sockets();

  static auto   make_server_request(std::string &&t_text_data) -> Request;

  void          retry(Request &t_req, bool got_res);
  void          hole_punch(Request &t_req);

  /* Constants */
  const inline static std::string m_DELIM            = " ";
  const inline static std::string m_LOCAL            = "LOCAL";
  const inline static std::size_t m_MAX_PING_TRIALS  = 5;
  const inline static std::size_t m_MAX_PUNCH_TRIALS = 20;
};

#endif
