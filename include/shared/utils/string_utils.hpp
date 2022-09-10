#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <tuple>

struct StringUtils {
  typedef std::vector<std::string> StringVector;
  typedef std::vector<std::size_t> IntVector;
  typedef std::tuple<std::string, std::string> StringTuple;
  typedef std::tuple<bool , int> IntTuple;
 

  /* split string by a delimiter 'delim' */
  static StringVector split(const std::string &s, const std::string &delim = " ");

  /* split a string in two at the first found occurance of a delimiter 'delim' */
  static StringTuple  split_first(const std::string &s, const std::string &delim = " ");

  /* Split a string with numeric values into a IntVector */
  static IntVector    split_to_ints(const std::string &s, const std::string &delim = " ");

  /* split a string at a certain position */
  static StringTuple  split_at(const std::string &s, const std::size_t t_pos);

  /* split a string by all positions of vector t_pos */
  static StringVector  split_at(const std::string &s, const IntVector &t_pos);

  /* joins a vector of string into a single string */
  static std::string  join(const StringVector &t_str_vec, const std::string &t_delim = " ");

  /* split a string by a delimiter 'delim' and return the last split item  */
  static std::string  last(const std::string &s, const std::string &delim = " ");

  /* converts a string numeric representation into an integer */
  static IntTuple     to_int(const std::string &s);
  
  /* manipulates the string in place */
  static void         to_upper(std::string &s);
  static void         trim(std::string &s);

private:
  static void         trim_left(std::string &s);
  static void         trim_right(std::string &s);
};

#endif // !UTILS_STRING_H
