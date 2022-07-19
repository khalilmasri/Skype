#include <iostream>
#include "logger.hpp"

int main(void){

    LOG_INFO("Hell client %s", "");
    std::cout << "Hello client" << std::endl;
    return 0;
}