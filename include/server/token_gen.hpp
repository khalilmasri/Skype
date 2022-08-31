#ifndef TOKEN_GENERATOR_H
#define TOKEN_GENERATOR_H

#include <string>

class TokenGenerator {

public:
  static std::string generate(const int length);

private:
  static char random_char();
};

#endif
