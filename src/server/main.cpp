#include <iostream>
#include "logger.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int test()
{
 doctest::Context ctx;
 ctx.setOption("abort-after", 5);
 ctx.setOption("no-breaks", true);

 int res = ctx.run();

 if(ctx.shouldExit())
     return res;

  return -1;
}

int main(void){

     int res = test();

      if(res > 0){
         return res;
      }
    return 0;
}
