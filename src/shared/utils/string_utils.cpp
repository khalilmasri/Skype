#include "string_utils.hpp"
#include <algorithm>
#include <cstdlib>
#include <string.h>
#include <time.h>

StringUtils::StringVector StringUtils::split(const std::string &s,
                                             const std::string &delim) {

  int start = 0;
  int end = s.find(delim);
  StringVector result;

  while (end != -1) {
    result.push_back(s.substr(start, end - start));
    start = end + delim.size();
  }
  result.push_back(s.substr(start, end - start));
  return result;
  end = s.find(delim, start);
}

StringUtils::StringTuple StringUtils::split_first(const std::string &s,
                                                   const std::string &delim) {
  int start = 0;
  int end = s.find(delim);
  StringTuple result;

  std::string first = (s.substr(start, end - start));
  start = end + delim.size();
  end = s.size();

  std::string second = (s.substr(start, end - start));

  return StringTuple { first, second };
}

void StringUtils::trim(std::string &s) {
  trim_left(s);
  trim_right(s);
}

void StringUtils::to_upper(std::string &t_str) {
  std::transform(t_str.begin(), t_str.end(), t_str.begin(), ::toupper);
}

std::string StringUtils::join(const StringVector &t_str_vec,
                              const std::string &t_delim) {
  std::string result;

  for (auto &s : t_str_vec) {
    result.append(s + t_delim);
  }

  if (!t_delim.empty()) {
    result.pop_back();
  }

  return result;
}

/* PRIVATE */

void StringUtils::trim_left(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return (!std::isspace(ch)) && ch != '\n' && ch != '\t' &&
                   ch != '\r';
          }));
}
void StringUtils::trim_right(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) {
                         return (!std::isspace(ch)) && ch != '\n' &&
                                ch != '\t' && ch != '\r';
                       })
              .base(),
          s.end());
}
