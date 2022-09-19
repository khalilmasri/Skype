#include "token_gen.hpp"
#include <string.h>
#include <algorithm>

std::string TokenGenerator::generate(const int length) {

  std::string str(length, 0);
  std::generate_n(str.begin(), length, random_char);

  return str;
}

char TokenGenerator::random_char(){
  const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    const size_t max_index = strlen(charset) - 1;
    return charset[rand() % max_index];
}
