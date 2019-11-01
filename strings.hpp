//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_STRINGS_H
#define DOUBANCRAWLER_STRINGS_H

#include <algorithm>
#include <iterator>
#include <sstream>

namespace doubanCrawler {
/**
 * String split function, use `delim` to split str, and then store the result into `cont`
 * @tparam Container container to hold splited results.
 * @param str the string need to split.
 * @param cont container
 * @param delim delimeter
 */
template <class Container>
void split(const std::string &str, Container &cont, char delim = ' ') {
  std::size_t current, previous = 0;
  current = str.find(delim);
  while (current != std::string::npos) {
    cont.insert(str.substr(previous, current - previous));
    previous = current + 1;
    current = str.find(delim, previous);
  }
  cont.insert(str.substr(previous, current - previous));
}

// Check if `source` starts with `prefix`
inline bool startsWith(const std::string &prefix, const std::string &source) {
  return source.rfind(prefix, 0) == 0;
}

// Check if `source` starts with `prefix`
inline bool startsWith(const std::string &prefix, const std::string &source,
                       size_t begin, size_t end) {
  return startsWith(prefix, source.substr(begin, end));
}

// Check if `source` contains `substring`
inline bool contains(const std::string &substring, const std::string &source) {
  return (source.find(substring) != std::string::npos);
}
}  // namespace doubanCrawler
#endif  // DOUBANCRAWLER_STRINGS_H
