#define DOCTEST_CONFIG_IMPLEMENT

#include "active_conn.hpp"
#include "config.hpp"
#include "doctest.h"
#include "peer_to_peer.hpp"
#include "request.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "text_io.hpp"
#include <iostream>

static Config *config = Config::get_instance();

std::string login_as(ActiveConn &conn, const char *user, const char *pw) {

  std::string addr = config->get<const std::string>("SERVER_ADDRESS");
  Request req = conn.connect_socket(addr);

  if (!req.m_valid) {
    std::cout << "YOU MUST HAVE THE SERVER RUNNING TO RUN THIS TEST."
              << std::endl;
    return {};
  }

  conn.receive(req); // ignore handshake

  req.set_data(new TextData(std::string("LOGIN ") + user + " " + pw));
  conn.respond(req);
  conn.receive(req);

  std::string response = TextData::to_string(req.data());

  auto [_, token] = StringUtils::split_first(response);

  return token;
}

void logout(ActiveConn &conn, std::string &token) {
  Request req;
  req.m_valid = true;
  req.set_data(new TextData(std::string("EXIT") + " " + token));
  conn.respond(req);
}

void connect_to(P2P &p2p) {

  std::string id = "3";
  p2p.connect_peer(id);

  if (p2p.status() != P2P::Awaiting) {
    std::cout << "did not call connect correctly. Exiting...\n";

    return;
  }

  int count = 0;

  while (p2p.status() != P2P::Accepted) {

    std::cout << "Pinging ...\n";

    p2p.ping_peer();
    sleep(2); // check every  2 second

    if (p2p.status() == P2P::Awaiting) {
      std::cout << "Still Awaiting ...\n";
    }

    if (p2p.status() == P2P::Error) {
      std::cout << "ping returned an error. exiting....\n";
      break;
    }

    if(count > 10){
      std::cout << std::string("Breaking after ") + std::to_string(count) << std::endl;
      p2p.hangup_peer();
      return;
    }

    count++;
  }

  std::cout << "connect Accepted sucessfully!\n";
}

void accept_from(P2P &p2p) {

  std::string id = "1";

  p2p.accept_peer(id);

  if (p2p.status() == P2P::Accepted) {
    std::cout << "Accepted was sucessful\n";
  } else {

    std::cout << "Accept failed.\n";
  }
}

int main(int ac, char **av) {

  if (ac < 3) {
    std::cout << "Enter a username and password.\n ./p2p_test john 1234\n";
    return 0;
  }

  std::string user(av[1]);

  std::cout << user << "\n";

  ActiveConn conn(config->get<int>("TCP_PORT"), new TextIO());
  std::string token = login_as(conn, av[1], av[2]);

  P2P p2p(token);

  if (user == "john") { // john connects
    connect_to(p2p);
  } else if (user == "shakira") { // shakira accepts
    accept_from(p2p);
  } else {
    std::cout << "Please pass in 'john' or 'shakira' as users.\n";
  }

  p2p.handshake_peer();

  logout(conn, token);

  config->free_instance();
  return 0;
}
