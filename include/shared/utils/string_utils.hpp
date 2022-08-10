#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <tuple>

struct StringUtils {
  typedef std::vector<std::string> StringVector;
  typedef std::tuple<std::string, std::string> StringTuple;

  static StringVector split(const std::string &s, const std::string &delim = " ");
  static StringTuple split_first(const std::string &s, const std::string &delim = " ");
  static std::string  join(const StringVector &t_str_vec, const std::string &t_delim = " ");
  static std::string  last(const std::string &s, const std::string &delim = " ");
  
  /* manipulates the string in place */
  static void         to_upper(std::string &s);
  static void         trim(std::string &s);

private:
  static void         trim_left(std::string &s);
  static void         trim_right(std::string &s);
};

#endif // !UTILS_STRING_H
