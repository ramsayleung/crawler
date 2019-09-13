//
// Created by Ramsay on 2019/9/9.
//

#ifndef DOUBANCRAWLER_STRINGS_H
#define DOUBANCRAWLER_STRINGS_H

#include <sstream>
#include <algorithm>
#include <iterator>

namespace doubanCrawler {
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

inline bool startsWith(const std::string &prefix, const std::string &source) {
  return source.rfind(prefix, 0) == 0;
}

inline bool startsWith(const std::string &prefix, const std::string &source, size_t begin, size_t end) {
  return startsWith(prefix, source.substr(begin, end));
}
}
#endif // DOUBANCRAWLER_STRINGS_H
