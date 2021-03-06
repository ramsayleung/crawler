///
/// Created by Ramsay on 2019/9/9.
///

#ifndef DOUBANCRAWLER_HTML_H
#define DOUBANCRAWLER_HTML_H

#include "dom.hpp"
#include <memory>
#include <set>
#include <string>

namespace crawler {

class Parser {
public:
  Parser(size_t pos, std::string input);
  /// Parse a sequence of sibling nodes.
  std::vector<crawler::Node>
  parseNodes(const std::shared_ptr<crawler::Node> &parent);

  /// Parse a sequence of sibling nodes.
  std::vector<crawler::Node> parseNodes();

  /// Parse a single node.
  crawler::Node parseNode();

  /// Parse a single node.
  crawler::Node parseNode(const std::shared_ptr<crawler::Node> &parent);

  /// Parse a single element, including its open tag, content, and closing tag.
  crawler::Node parseElement();

  /// Parse a single element, including its open tag, content, and closing tag.
  crawler::Node parseElement(std::shared_ptr<crawler::Node> parent);

private:
  size_t pos;
  std::string input;
  inline static const std::set<std::string> SELF_CLOSING_TAGS = {
      "area",   "base", "br",       "col",  "embed", "hr",     "img",   "input",
      "keygen", "link", "menuitem", "meta", "param", "source", "track", "wbr"};

  /// Parse a tag or attribute name.
  std::string parseTagName();

  /// Parse a list of name="values" pairs, separated by whitespace.
  crawler::AttrMap parseAttributes();

  /// Parse a single name="value" pair.
  std::pair<std::string, std::string> parseAttribute();

  /// Parse a quoted value.
  std::string parseAttributeValue();

  /// Parse a text node
  crawler::Node parseText(const std::shared_ptr<crawler::Node> &parent);

  /// Consume and discard zero or more whitespace characters
  void consumeWhitespace();

  /// Consume and discard zero or more comment
  void consumeComment();

  /// Consume and discard doctype.
  void consumeDoctype();

  /// Consume character until `test` return false.
  template <class Predicate> std::string consumeWhile(Predicate predicate);

  /// Return the current character and advance `pos` to the next character
  char consumeChar();

  /// Read the current character without consuming it.
  char nextChar();

  /// Does the current input start with the given string
  bool startsWith(const std::string &prefix);

  /// Return true if all input is consumed.
  bool eof();

  /// Check if `currentTagName` is a self-closing tag or not.
  [[nodiscard]] bool isSelfClosingTag(const std::string &currentTagName) const;
};

crawler::Node parse(const std::string &source);

} // namespace crawler

#endif // DOUBANCRAWLER_HTML_H
