#ifndef CLIENT_STREAM_H
#define CLIENT_STREAM_H
#include "peer_to_peer.hpp"


class ClientStream {

  public:
  ClientStream(P2P *p2p_conn);

  void out(Data *data);
  Data *in();


  private:
  P2P *m_p2p_conn;
  Request m_req;

};
#endif

