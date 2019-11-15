//
// Created by Ramsay on 2019/9/9.
//

#include "html.hpp"
#include "dom.hpp"
#include <cassert>
#include <memory>
#include <regex>
#include <utility>
#include <vector>
namespace crawler {

bool crawler::Parser::startsWith(const std::string &prefix) {
  return crawler::startsWith(prefix, input, pos, input.size());
}

bool crawler::Parser::eof() { return pos >= input.size(); }

bool crawler::Parser::isSelfClosingTag(
    const std::string &currentTagName) const {
  return SELF_CLOSING_TAGS.find(currentTagName) != SELF_CLOSING_TAGS.end();
}

char crawler::Parser::nextChar() { return input.c_str()[pos]; }

char crawler::Parser::consumeChar() {
  char currentChar = input.c_str()[pos];
  pos += 1;
  return currentChar;
}

template <class Predicate>
std::string crawler::Parser::consumeWhile(Predicate predicate) {
  std::string result;
  while (!eof() && predicate(nextChar())) {
    result += consumeChar();
  }
  return result;
}

void crawler::Parser::consumeWhitespace() {
  crawler::Parser::consumeWhile([](char c) -> bool { return isspace(c); });
}

// delete all comment tag
void crawler::Parser::consumeComment() {
  std::string commentDescriptor("<!--");
  if (!crawler::contains(commentDescriptor, input)) {
    return;
  }
  std::string emptyString;
  std::regex pattern(R"(<!--.*?-->)");
  std::regex_replace(input, pattern, emptyString);
}

void Parser::consumeDoctype() {
  std::string doctypeIdentifier("<!doctype");
  if (!crawler::containsIgnoreCase(doctypeIdentifier, input)) {
    return;
  }
  std::string emptyString;
  std::regex pattern(R"(<!.*?>)");
  input = std::regex_replace(input, pattern, emptyString);
}

crawler::Node
crawler::Parser::parseText(const std::shared_ptr<crawler::Node> &parent) {
  return crawler::Node(consumeWhile([](char c) -> bool { return c != '<'; }),
                       parent);
}

std::string crawler::Parser::parseAttributeValue() {
  const char openQuote = consumeChar();
  assert(openQuote == '"' || openQuote == '\'');
  auto value =
      consumeWhile([&openQuote](char c) -> bool { return c != openQuote; });
  assert(openQuote == consumeChar());
  return value;
}

std::pair<std::string, std::string> crawler::Parser::parseAttribute() {
  const std::string name = parseTagName();
  assert(consumeChar() == '=');
  const std::string value = parseAttributeValue();
  return std::make_pair(name, value);
}

std::string crawler::Parser::parseTagName() {
  return consumeWhile([](char c) -> bool { return isalnum(c); });
}
crawler::AttrMap crawler::Parser::parseAttributes() {
  std::map<std::string, std::string> attributes;
  while (true) {
    consumeWhitespace();
    if (nextChar() == '>') {
      break;
    }
    std::pair<std::string, std::string> attribute = parseAttribute();
    attributes.insert(attribute);
  }
  return attributes;
}
crawler::Node crawler::Parser::parseElement() { return parseElement(nullptr); }
crawler::Node
crawler::Parser::parseElement(const std::shared_ptr<crawler::Node> parent) {
  // Opening tag.
  assert(consumeChar() == '<');
  const std::string tagName = parseTagName();
  const std::map<std::string, std::string> attributes = parseAttributes();
  const std::vector<crawler::Node> emptyChild;
  if (isSelfClosingTag(tagName)) {
    // self-closing has various format, such as <img> or <img/>
    if (nextChar() == '>') {
      assert(consumeChar() == '>');
    } else {
      assert(consumeChar() == '/');
      assert(consumeChar() == '>');
    }
    return crawler::Node(tagName, attributes, emptyChild, parent);
  } else {
    assert(consumeChar() == '>');
  }
  crawler::Node currentNode(tagName, attributes, emptyChild, parent);
  auto ptr = std::make_shared<crawler::Node>(currentNode);
  // Contents.
  const std::vector<crawler::Node> children = parseNodes(ptr);
  // Closing tag.
  assert(consumeChar() == '<');
  assert(consumeChar() == '/');
  assert(parseTagName() == tagName);
  assert(consumeChar() == '>');
  crawler::Node node(tagName, attributes, children, parent);
  return node;
}

std::vector<crawler::Node> crawler::Parser::parseNodes() {
  return parseNodes(nullptr);
}

std::vector<crawler::Node>
crawler::Parser::parseNodes(const std::shared_ptr<crawler::Node> &parent) {
  consumeComment();
  consumeDoctype();
  std::vector<crawler::Node> nodes;
  while (true) {
    consumeWhitespace();
    if (eof() || startsWith("</")) {
      break;
    }
    nodes.emplace_back(parseNode(parent));
  }
  return nodes;
}

crawler::Node crawler::Parser::parseNode() { return parseNode(nullptr); }

crawler::Node
crawler::Parser::parseNode(const std::shared_ptr<crawler::Node> &parent) {
  if (nextChar() == '<') {
    return parseElement(parent);
  } else {
    return parseText(parent);
  }
}

crawler::Parser::Parser(size_t _pos, std::string _input) {
  pos = _pos;
  input = std::move(_input);
}

crawler::Node parse(const std::string &source) {
  const std::vector<crawler::Node> nodes =
      crawler::Parser(0, source).parseNodes(nullptr);

  // If the document contains a root element, just return it, Otherwise, create
  // one.
  if (nodes.size() == 1) {
    return nodes.at(0);
  } else {
    const std::string html("html");
    const crawler::AttrMap attributes;
    return crawler::Node(html, attributes, nodes, nullptr);
  }
}
} // namespace crawler
