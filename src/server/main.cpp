#include "logger.hpp"
#include <iostream>
#define DOCTEST_CONFIG_IMPLEMENT
#include "server.hpp"
#include "tester.hpp"

#define PORT 5000

int main(int ac, char *av[]) {

  int res = Tester::test(ac, av);

  if (res > 0) {
    return res;
  }

  Server server(PORT);
  server.spawn_udp_listener();
  server.main_loop();

  return 0;
}
