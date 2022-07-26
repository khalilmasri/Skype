#include "conf.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>


Config::Config() {

   bool res = load();
}

bool Config::load(){

   std::ifstream file;
   std::string line;
   int count_lines = 0;

   bool res = file.good();
   if ( true == res){
      while (std::getline(file,line)){
         std::cout << line << std::endl;
      }
      file.close();
   }

   return res;
}
