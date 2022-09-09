#include <iostream>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT

#include "active_conn.hpp"
#include "doctest.h"
#include "logger.hpp"
#include "request.hpp"
#include "string_utils.hpp"
#include "text_data.hpp"
#include "text_io.hpp"
#include "user_chat.hpp"
#include "udp_conn.hpp"
#include "config.hpp"

static Config *config = Config::get_instance();

void test_tcp() {

  ActiveConn conn(5000, new TextIO());

  std::cout << "Testing TCP connection." << std::endl;

  std::cout << "attempting to connect to server..." << std::endl;
  // >>>>>>> client_to_server
  std::string addr = config->get<const std::string>("SERVER_ADDRESS");
  // std::string addr = "127.0.0.1";
  Request req = conn.connect_socket(addr);

  if (!req.m_valid) {
    std::cout << "Server is not running on " + addr +
                     ". Make sure you start the server before running this "
                     "tester app."
              << std::endl;
  }

  else {
    std::cout << "Connected. Waiting for server response..." << std::endl;
    conn.receive(req);
    std::string handshake = TextData::to_string(req.data());
    std::cout << handshake << std::endl;

    for (std::string line; std::getline(std::cin, line);) {

      auto [c, _] = StringUtils::split_first(line);
      if (line == "quit") {

        req.set_data(new TextData("EXIT"));
        conn.respond(req);
        break;
      }

      req.set_data(new TextData(line));
      conn.respond(req);
      conn.receive(req);

      std::string response = TextData::to_string(req.data());

      std::cout << response << std::endl;
    };
  }
}

void test_udp() {

    std::cout << "Testing UDP connection." << std::endl;

    auto conn = UDPConn(5001, new TextIO());
    std::string addr = "127.0.0.1";
    conn.bind_socket(addr);
    Request req;

    req.set_data(new TextData("hello from client"));
    conn.respond(req);

    conn.receive(req);

    std::cout << TextData::to_string(req.data());

}

int main(int ac, char **av) {

  if (ac > 1 && std::string(av[1]) == "TCP") {
    test_tcp();
  } else if (ac > 1 && std::string(av[1]) == "UDP") {

    test_udp();

  } else {
    test_tcp();
  }
}
