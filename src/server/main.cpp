#include <iostream>
#include "logger.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "server.hpp"

#define PORT 5000

bool do_test(std::string &&t_av){
   if(t_av == "--test"){
     return true;
   }
   return false;
}

int test(bool do_test)
{
  if(!do_test){
    return 0;
  }

 doctest::Context ctx;
 ctx.setOption("abort-after", 5);
 ctx.setOption("no-breaks", true);

 int res = ctx.run();

 if(ctx.shouldExit()){
     return res;
 }

  return -1;
}


int main(int ac, char *av[]){

     int res = test(ac > 1 && do_test(std::string(av[1])));

      if(res > 0){
         return res;
      }

    Server server(PORT);
    server.main_loop();

    return 0;
}
