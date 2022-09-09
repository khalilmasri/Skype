#ifndef TESTER_H
#define TESTER_H

#include "doctest.h"
#include <string>

struct Tester {

  static int test(int argc, char *argv[]) {
    return do_test(argc > 1 && will_test(std::string(argv[1])));
  }

private:
  static bool will_test(std::string &&t_av) {
    if (t_av == "--test") {
      return true;
    }
    return false;
  }

  static int do_test(bool t_will_test) {
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
