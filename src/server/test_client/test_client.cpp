#include <iostream>
#include <string>
#define DOCTEST_CONFIG_IMPLEMENT

#include "active_conn.hpp"
#include "doctest.h"
#include "request.hpp"
#include "text_data.hpp"
#include "text_io.hpp"

int main(void) {

  ActiveConn conn(5000, new TextIO());

  std::cout << "attempting to connect to server..." << std::endl;
  // this is the server address
  std::string addr = "206.189.0.154";
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
      if (line == "quit") {
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
