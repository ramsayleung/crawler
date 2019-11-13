//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_STRINGS_H
#define DOUBANCRAWLER_STRINGS_H

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

namespace crawler {

/// String split function, use `delim` to split str, and then store the result
/// into `cout`
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

/// Find index of subString in `source`.
inline size_t indexOf(const std::string &subString, const std::string &source) {
  return source.find(subString);
} // namespace crawler

/// Convert `input` to lower case and trim it.
inline std::string normalize(const std::string &input) {
  std::string buffer = input;
  std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
  return buffer;
} // namespace crawler

/// Check if `source` starts with `prefix`
inline bool startsWith(const std::string &prefix, const std::string &source) {
  //  return source.rfind(prefix, 0) == 0;
  return source.size() >= prefix.size() &&
         (source.compare(0, prefix.size(), prefix) == 0);
}

/// Check if `source` ends with `suffix`
inline bool endsWith(const std::string &suffix, const std::string &source) {
  return source.size() >= suffix.size() &&
         0 == source.compare(source.size() - suffix.size(), suffix.size(),
                             suffix);
}

/// Check if `source` starts with `prefix`
inline bool startsWith(const std::string &prefix, const std::string &source,
                       size_t begin, size_t end) {
  return startsWith(prefix, source.substr(begin, end));
}

/// Check if `source` contains `substring`
inline bool contains(const std::string &substring, const std::string &source) {
  return (source.find(substring) != std::string::npos);
}

/// Compares this `source` to another `target`, ignoring case considerations.
inline bool containsIgnoreCase(const std::string &source,
                               const std::string &target) {
  return contains(normalize(source), normalize(target));
}

} // namespace crawler
#endif // DOUBANCRAWLER_STRINGS_H
