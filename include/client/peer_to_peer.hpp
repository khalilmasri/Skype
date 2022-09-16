#ifndef PEER_TO_PEER_H
#define PEER_TO_PEER_H
#include "udp_conn.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "local_ip.hpp"
#include "server_commands.hpp"

// TODO: Test peer handshake in two computers. 

class P2P {

  public:
  enum Status {Idle, Awaiting, Accepted, Connected, Error};
  enum Type   { Initiator, Acceptor, None};

  P2P(std::string &t_token) noexcept;
  P2P(std::string &&t_token) noexcept;

  /* getters */
  Status        status() const;
  std::string   status_to_string() const;
  Type          type() const;
  std::string   type_to_string() const;
  Reply::Code   last_reply() const;

  /* Reset the P2P connection */
  void          reset();

  /* Peers are ready to connect to each other */
  void          handshake_peer();
  void          stream_out();
  void          stream_in();

  /* Calls to the server to connect to another peer */
  void          connect_peer(std::string &t_peer_id);
  void          accept_peer(std::string &t_peer_id);
  void          reject_peer(std::string &t_peer_id);
  void          ping_peer();
  void          hangup_peer();

  private:
  enum PeerNetwork {Local, Web};

  std::string   m_peer_address;
  std::string   m_token;
  UDPConn       m_inbounds;
  UDPConn       m_outbounds; // NOTE: Not yet implemented. Still looking to see if will be needed.
  Status        m_status;
  Type          m_type;
  Reply::Code   m_last_reply;
  LocalIP       m_local_ip;

  /* return the res message from the Server and sets m_last_reply with the Reply::Code */
  std::string   send_server(ServerCommand::name t_cmd, std::string &t_arg);
  std::string   send_server(ServerCommand::name t_cmd);
  std::string   send_server(std::string &&t_text_data);

  void          handshake_acceptor(Request &t_req, PeerNetwork t_peer_network);
  void          handshake_initiator(Request &t_req , PeerNetwork t_peer_network);

  bool          has_same_address();
  bool          invalid_to_handshake();

  Request       make_server_request(std::string &&t_text_data);
  void          bind_sockets();

  void          hole_punch(Request &t_req);

  


  const static std::string m_DELIM;
};

#endif
