//
// Created by Ramsay on 2019/9/9.
//
#include "test.hpp"
#include "dom.hpp"
#include "html.hpp"
#include "http.hpp"
#include "selector.hpp"
#include "utils.hpp"

#include <fstream>
#include <regex>
#include <string>

void printNode(const crawler::Node &result) {
  TRACE(("tagName: %s\n", result.getNodeData().element.getTagName().c_str()));
  crawler::AttrMap attributes = result.getNodeData().element.getAttributes();
  for (auto const &keyValuePair : attributes) {
    TRACE(("key: %s value: %s\n", keyValuePair.first.c_str(),
           keyValuePair.second.c_str()));
  }
}

void testEqualMacro() {
  ASSERT_TRUE(true);
  ASSERT_CSTRING_EQ("div", "div");
}

void testSplit() {
  std::string classes = "item.pic.info";
  std::set<std::string> classSet;
  crawler::split(classes, classSet, '.');
  ASSERT_UNSIGNED_LONG_EQ(3lu, classSet.size());
}

void testStartsWith() {
  const std::string source = "tititoto";
  std::string prefix = "titi";
  ASSERT_INT_EQ(1, crawler::startsWith(prefix, source));
  prefix = "tito";
  ASSERT_INT_EQ(true, crawler::startsWith(prefix, source, 2, 6));
}

void testParseElement() {
  const std::string source = R"(<div id="main" class="test"></div>)";
  crawler::Parser parser(0, source);
  crawler::Node node = parser.parseElement();
  std::string tagName = node.getNodeData().element.getTagName();
  ASSERT_CSTRING_EQ("div", tagName.c_str());
}

void testParseNode() {
  const std::string source = R"(<p>hello world</p>)";
  crawler::Parser parser(0, source);
  crawler::Node node = parser.parseNode();
  ASSERT_TRUE(node.getNodeData().isElementData());
  ASSERT_TRUE(node.getChildren().size() == 1);
  crawler::Node childrenNode = node.getChildren().at(0);
  ASSERT_TRUE(childrenNode.getNodeData().isText());
  ASSERT_CSTRING_EQ("hello world", childrenNode.getNodeData().text.c_str());
}

void testParseAttributes() {
  const std::string source = R"(<div id="main" class="test"></div>)";
  crawler::Parser parser(0, source);
  std::vector<crawler::Node> nodes = parser.parseNodes();
  crawler::Node node = nodes.at(0);
  ASSERT_TRUE(node.getNodeData().isElementData());
}

void testParse() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
}

void testParseSelfClosingTag() {
  std::ifstream file("source/parseSelfClosingTagTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes elements = node.getElementsByTag(std::string("img"));
  auto const element = elements[0];
  const crawler::AttrMap attributes =
      element.getNodeData().element.getAttributes();
  auto iterator = attributes.find("width");
  ASSERT_TRUE(iterator != attributes.end());
  ASSERT_CSTRING_EQ("100", iterator->second.c_str());
}

void testStringContains() {
  std::string substring("needle");
  std::string source("There are two needles in this haystack.");
  ASSERT_TRUE(crawler::contains(substring, source));
}

void testConsumeComment() {
  std::ifstream file("source/commentTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string emptyString;
  std::regex pattern(R"(<!--.*?-->)");
  std::string result = std::regex_replace(buffer.str(), pattern, emptyString);
  std::string commentDescriptor("<!--");
  ASSERT_TRUE(!crawler::contains(commentDescriptor, result));
}

void testHttp() {
  /* first where are we going to send it? */
  const std::string host = "stackoverflow.com";
  crawler::http_get(host);
}

void testGetElementsByTag() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes elements = node.getElementsByTag(std::string("div"));
  for (auto const &result : elements) {
    printNode(result);
  }
}

void testGetElementById() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Node result = node.getElementById("main");
  printNode(result);
  ASSERT_CSTRING_EQ(
      result.getNodeData().element.getAttributes().at("class").c_str(), "test");
}

void testMatchesWhitespace() {
  crawler::TokenQueue tokenQueue(" test");
  ASSERT_TRUE(tokenQueue.matchesWhitespace());
}

void testMatch() {
  crawler::TokenQueue tokenQueue("to Tutorialspoint", 3);
  ASSERT_TRUE(tokenQueue.matches("Tutorials"));
}

void testMatchesChomp() {
  crawler::TokenQueue tokenQueue("to Tutorialspoint", 3);
  ASSERT_TRUE(tokenQueue.matchesChomp("Tutorials"));
  ASSERT_UNSIGNED_LONG_EQ(12UL, tokenQueue.getPos());
}

void testMatchesWords() {
  crawler::TokenQueue tokenQueue("a[href]");
  ASSERT_TRUE(tokenQueue.matchesWords());
}

void testMatchesAny() {
  crawler::TokenQueue tokenQueue("a[href]");
  ASSERT_TRUE(tokenQueue.matchesAny(std::array<char, 2>({'b', 'a'})));
}

void testConsumeCssIdentifier() {
  crawler::TokenQueue tokenQueue("a[href]");
  ASSERT_CSTRING_EQ("a", tokenQueue.consumeCssIdentifier().c_str());
}

int main() {
  testConsumeCssIdentifier();
  testMatchesAny();
  testMatchesWords();
  testMatchesChomp();
  testMatch();
  testMatchesWhitespace();
  testGetElementsByTag();
  testGetElementById();
  testEqualMacro();
  testSplit();
  testStartsWith();
  testStringContains();
  testParseElement();
  testParseNode();
  testParseAttributes();
  testParse();
  testHttp();
  testConsumeComment();
  testParseSelfClosingTag();
  PRINT_TEST_RESULT();
}
