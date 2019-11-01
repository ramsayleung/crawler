///
/// Created by Ramsay on 2019/9/9.
///

#ifndef DOUBANCRAWLER_HTML_H
#define DOUBANCRAWLER_HTML_H

#include "dom.hpp"

namespace doubanCrawler {

class Parser {
 public:
  Parser(size_t pos, std::string input);
  /// Parse a sequence of sibling nodes.
  std::vector<doubanCrawler::Node> parseNodes();

  /// Parse a single node.
  doubanCrawler::Node parseNode();

  /// Parse a single element, including its open tag, content, and closing tag.
  doubanCrawler::Node parseElement();

  /// Parse a tag or attribute name.
  std::string parseTagName();

  /// Parse a list of name="values" pairs, separated by whitespace.
  doubanCrawler::AttrMap parseAttributes();

  /// Parse a single name="value" pair.
  std::pair<std::string, std::string> parseAttribute();

  /// Parse a quoted value.
  std::string parseAttributeValue();

  /// Parse a text node
  doubanCrawler::Node parseText();

  /// Consume and discard zero or more whitespace characters
  void consumeWhitespace();

  /// Consume and discard zero or more comment 
  void consumeComment();

  /// Consume character until `test` return false.
  template<class Predicate>
  std::string consumeWhile(Predicate predicate);

  /// Return the current character and advance `pos` to the next character
  char consumeChar();

  /// Read the current character without consuming it.
  char nextChar();

  /// Does the current input start with the given string
  bool startsWith(const std::string &prefix);

  /// Return true if all input is consumed.
  bool eof();
 private:
  size_t pos;
  std::string input;
};

doubanCrawler::Node parse(const std::string &source);

}

#endif // DOUBANCRAWLER_HTML_H
