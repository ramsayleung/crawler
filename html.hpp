///
/// Created by Ramsay on 2019/9/9.
///

#ifndef DOUBANCRAWLER_HTML_H
#define DOUBANCRAWLER_HTML_H

#include <set>
#include <string>
#include "dom.hpp"

namespace crawler {

class Parser {
 public:
  Parser(size_t pos, std::string input);
  /// Parse a sequence of sibling nodes.
  std::vector<crawler::Node> parseNodes();

  /// Parse a single node.
  crawler::Node parseNode();

  /// Parse a single element, including its open tag, content, and closing tag.
  crawler::Node parseElement();

  /// Parse a tag or attribute name.
  std::string parseTagName();

  /// Parse a list of name="values" pairs, separated by whitespace.
  crawler::AttrMap parseAttributes();

  /// Parse a single name="value" pair.
  std::pair<std::string, std::string> parseAttribute();

  /// Parse a quoted value.
  std::string parseAttributeValue();

  /// Parse a text node
  crawler::Node parseText();

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

  /// Check if `currentTagName` is a self-closing tag or not.
  bool isSelfClosingTag(const std::string& currentTagName);
 private:
  size_t pos;
  std::string input;
  inline static const std::set<std::string> SELF_CLOSING_TAGS = {"area", "base", "br", "col", "embed", "hr", "img", "input", "keygen", "link", "menuitem","meta", "param", "source", "track", "wbr"};
};

crawler::Node parse(const std::string &source);

}

#endif // DOUBANCRAWLER_HTML_H
