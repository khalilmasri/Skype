#define DOCTEST_CONFIG_IMPLEMENT
#include "server.hpp"
#include "tester.hpp"
#include "config.hpp"

int main(int ac, char *av[]) {

  Config *config = Config::get_instance();

  srand(time(nullptr)); // seed srand for token generator

  int res = Tester::test(ac, av);

  if (res > 0) {
    return res;
  }

  Server server(config->get<int>("TCP_PORT"));
  server.spawn_udp_listener();
  server.main_loop();

  config->free_instance();

  return 0;
}
