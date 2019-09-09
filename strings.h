//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_STRINGS_H
#define DOUBANCRAWLER_STRINGS_H

#include <sstream>
#include <algorithm>
#include <iterator>

namespace String {

/**
 * split the string and insert the result into container
 * @tparam Container something to hold the result
 * @param str the source string
 * @param cont container
 * @param delim delimiter
 */
template<class Container>
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

bool startsWith(const std::string &prefix, const std::string &source) {
  return source.rfind(prefix, 0) == 0;
}

bool startsWith(const std::string &prefix, const std::string &source, size_t begin, size_t end) {
  return startsWith(prefix, source.substr(begin, end));
}
}
#endif //DOUBANCRAWLER_STRINGS_H
