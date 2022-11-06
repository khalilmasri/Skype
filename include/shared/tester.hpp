#ifndef TESTER_H
#define TESTER_H

#include "doctest.h"
#include <string>
#include <iostream>

struct Tester {

  static auto test(int argc, char **argv) -> int {
    return do_test(argc > 1 && (std::string(argv[1]) == "--test"));
  }

  static auto did_test(int argc, char **argv) -> bool {
    return argc > 1 && will_test(std::string(argv[1]));
  }

private:
  static auto will_test(std::string &&t_av) -> bool {
    return t_av == "--test";
  }

  static auto do_test(bool t_will_test) -> int {
    if (!t_will_test) {
      return 0;
    }

    doctest::Context ctx;
    ctx.setOption("abort-after", 5);
    ctx.setOption("no-breaks", true);

    int res = ctx.run();

    if (ctx.shouldExit()) {
      return res;
    }

    return -1;
  }
};

#endif // !TESTER_H
