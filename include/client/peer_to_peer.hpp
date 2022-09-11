#ifndef PEER_TO_PEER_H
#define PEER_TO_PEER_H
#include "udp_conn.hpp"
#include "request.hpp"
#include "reply.hpp"
#include "server_commands.hpp"

class P2P {

  public:
  enum Status {Idle, Awaiting, Accepted, Connected, Error};

  P2P(std::string &t_token) noexcept;
  P2P(std::string &&t_token) noexcept;

  void connect_peer(std::string &t_peer_id);
  void accept_peer(std::string &t_peer_id);
  void reject_peer(std::string &t_peer_id);
  void ping_peer();
  void hangup_peer();

  Status        status() const;
  Reply::Code   last_reply() const;
  void          reset();

  private:
   std::string  m_peer_address;
   std::string  m_token;
   UDPConn      m_inbounds;
   UDPConn      m_outbounds;
   Status       m_status;
   Reply::Code  m_last_reply;

   std::string send_server(ServerCommand::name t_cmd, std::string &t_arg);
   std::string send_server(ServerCommand::name t_cmd);
   std::string send_server(std::string &&t_text_data);

   Request     make_server_request(std::string &&t_text_data);
   void        bind_sockets();


   const static std::string m_DELIM;
};

#endif
