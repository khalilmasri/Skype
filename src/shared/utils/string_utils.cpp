#include "string_utils.hpp"
#include "logger.hpp"
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
    end = s.find(delim, start);
  }
  result.push_back(s.substr(start, end - start));

  return result;
}

StringUtils::IntVector StringUtils::split_to_ints(const std::string &s,
                                                  const std::string &delim) {
  int start = 0;
  int end = s.find(delim);
  IntVector result;

  try {
    while (end != -1) {
      std::size_t val = std::stoi(s.substr(start, end - start));
      result.push_back(val);
      start = end + delim.size();
      end = s.find(delim, start);
    }

    std::size_t val = std::stoi(s.substr(start, end - start));
    result.push_back(val);
  } catch (...) {
    LOG_ERR("Attempted to split containing Integer but found non-numeric values.");
    return IntVector();
  }

  return result;
}

std::string StringUtils::last(const std::string &s, const std::string &delim) {

  int start = 0;
  int end = s.find(delim);
  std::string result;

  while (end != -1) {
    start = end + delim.size();
  }

  result = s.substr(start, end - start);

  return result;
}

StringUtils::StringTuple StringUtils::split_first(const std::string &s,
                                                  const std::string &delim) {

  if (s.empty()) {
    return StringTuple{"", ""};

  }
  int start = 0;
  unsigned long end = s.find(delim);

  if(end == std::string::npos){
    return StringTuple{s, ""};
  }

  StringTuple result;

  std::string first = (s.substr(start, end - start));
  start = end + delim.size();
  end = s.size();

  std::string second = (s.substr(start, end - start));

  return StringTuple{first, second};
}

StringUtils::StringTuple StringUtils::split_at(const std::string &s,
                                               const std::size_t t_pos) {
  return StringTuple{s.substr(0, t_pos), s.substr(t_pos, s.size())};
}

StringUtils::StringVector StringUtils::split_at(const std::string &s, const IntVector &t_pos) {

  StringVector vec;

  for (std::size_t i = 0; i < t_pos.size() - 1; i++) {
    int start = t_pos.at(i);
    int end = t_pos.at(i + 1) - t_pos.at(i);
    vec.push_back(s.substr(start, end));
  }
      
  return vec;
}


StringUtils::IntTuple StringUtils::to_int(const std::string &s){

  try{
    int val = std::stoi(s);
    return {true, val};

  } catch(...){
     LOG_ERR("Could not convert %s into an integer.", s.c_str());
     return {false, -1};
  }
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
