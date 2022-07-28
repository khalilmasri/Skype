#ifndef SERVER_H
#define SERVER_H

#include "passive_conn.hpp"
#include "text_io.hpp"
#include <string>

class Server {

  PassiveConn m_conn;
  std::string m_address;

  Server(int t_port): m_conn(t_port, new TextIO()), m_address("127.0.0.1"){
    m_conn.setup(m_address);
  }

};


#endif

// commands
//
// CREATE <user> <password> (do we wan't create? )
// LOGIN <user> <password>
// LIST  (list your all contacts with their status and current ip) 
// SEARCH <user> (searches the whole user list in the server for a given username ).
// ADD   <user>
// REMOVE <user>
// PING  (it might be a good idea for the client to ping every second or so to confirm a client is still online )
// AVAILABLE <user> (we can use this for a quick check on a given user/conect status and IP before connecting )



// Reply codes
// 200 OK
// 201 <data result> (from LIST, SEARCH and AVAILABLE for example)

// 300 NOT OK
// 301 NOT FOUND

// 500 Internal Server Error
// 501 Invalid Command

// Responses

// CREATE LOGIN ADD REMOVE PING
//  200 OK
//  300 NOT OK

// AVAILABLE SEARCH
// 201 <user>:<address>:<status>
// 301 NOT FOUND

// lIST
// 201 <user>:<address>:<status>,<user>:<address>:<status>
// 301 NOT FOUND
// e.g.
//   201 khalil:125.43.32.1:up,chris:125.43.32.1:down,shakira:123.34.43.1:down



