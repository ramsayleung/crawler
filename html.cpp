//
// Created by Ramsay on 2019/9/9.
//

#include "html.hpp"
#include <cassert>
#include <regex>
#include <utility>
namespace doubanCrawler {

bool doubanCrawler::Parser::startsWith(const std::string &prefix) {
    return doubanCrawler::startsWith(prefix, input, pos, input.size());
}

bool doubanCrawler::Parser::eof() {
    return pos >= input.size();
}

char doubanCrawler::Parser::nextChar() {
    return input.c_str()[pos];
}

char doubanCrawler::Parser::consumeChar() {
    char currentChar = input.c_str()[pos];
    pos += 1;
    return currentChar;
}

template <class Predicate>
std::string doubanCrawler::Parser::consumeWhile(Predicate predicate) {
    std::string result;
    while (!eof() && predicate(nextChar())) {
        result += consumeChar();
    }
    return result;
}

void doubanCrawler::Parser::consumeWhitespace() {
    doubanCrawler::Parser::consumeWhile([](char c) -> bool { return isspace(c); });
}

// delete all comment tag
void doubanCrawler::Parser::consumeComment() {
    std::string commentDescriptor("<!--");
    if (!doubanCrawler::contains(commentDescriptor, input)) {
        return;
    }
    std::string emptyString("");
    std::regex pattern(R"(<!--.*?-->)");
    std::regex_replace(input, pattern, emptyString);
}

doubanCrawler::Node doubanCrawler::Parser::parseText() {
    return doubanCrawler::Node(consumeWhile([](char c) -> bool { return c != '<'; }));
}

std::string doubanCrawler::Parser::parseAttributeValue() {
    const char openQuote = consumeChar();
    assert(openQuote == '"' || openQuote == '\'');
    auto value = consumeWhile([&openQuote](char c) -> bool { return c != openQuote; });
    assert(openQuote == consumeChar());
    return value;
}

std::pair<std::string, std::string> doubanCrawler::Parser::parseAttribute() {
    const std::string name = parseTagName();
    assert(consumeChar() == '=');
    const std::string value = parseAttributeValue();
    return std::make_pair(name, value);
}

std::string doubanCrawler::Parser::parseTagName() {
    return consumeWhile([](char c) -> bool { return isalnum(c); });
}
doubanCrawler::AttrMap doubanCrawler::Parser::parseAttributes() {
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
doubanCrawler::Node doubanCrawler::Parser::parseElement() {
    // Opening tag.
    assert(consumeChar() == '<');
    const std::string tagName = parseTagName();
    const std::map<std::string, std::string> attributes = parseAttributes();
    assert(consumeChar() == '>');

    // Contents.
    const std::vector<doubanCrawler::Node> children = parseNodes();
    // Closing tag.
    assert(consumeChar() == '<');
    assert(consumeChar() == '/');
    assert(parseTagName() == tagName);
    assert(consumeChar() == '>');
    return doubanCrawler::Node(tagName, attributes, children);
}
std::vector<doubanCrawler::Node> doubanCrawler::Parser::parseNodes() {
    consumeComment();
    std::vector<doubanCrawler::Node> nodes;
    while (true) {
        consumeWhitespace();
        if (eof() || startsWith("</")) {
            break;
        }
        nodes.emplace_back(parseNode());
    }
    return nodes;
}
doubanCrawler::Node doubanCrawler::Parser::parseNode() {
    if (nextChar() == '<') {
        return parseElement();
    } else {
        return parseText();
    }
}

doubanCrawler::Parser::Parser(size_t _pos, std::string _input) {
    pos = _pos;
    input = std::move(_input);
}

doubanCrawler::Node parse(const std::string &source) {
    const std::vector<doubanCrawler::Node> nodes = doubanCrawler::Parser(0, source).parseNodes();

    // If the document contains a root element, just return it, Otherwise, create one.
    if (nodes.size() == 1) {
        return nodes.at(0);
    } else {
        const std::string html("html");
        const doubanCrawler::AttrMap attributes;
        return doubanCrawler::Node(html, attributes, nodes);
    }
}
}  // namespace doubanCrawler
