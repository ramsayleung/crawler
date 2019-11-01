//
// Created by Ramsay on 2019/9/9.
//
#include "test.hpp"
#include "dom.hpp"
#include "html.hpp"
#include "http.hpp"
#include "utils.hpp"

#include <fstream>
#include <regex>
#include <string>

void testEqualMacro() {
  EXPECT_EQ_TRUE(true);
  EXPECT_EQ_CSTRING("div", "div");
}

void testSplit() {
  std::string classes = "item.pic.info";
  std::set<std::string> classSet;
  doubanCrawler::split(classes, classSet, '.');
  EXPECT_EQ_UNSIGNED_LONG(3lu, classSet.size());
}

void testStartsWith() {
  const std::string source = "tititoto";
  std::string prefix = "titi";
  EXPECT_EQ_INT(1, doubanCrawler::startsWith(prefix, source));
  prefix = "tito";
  EXPECT_EQ_INT(true, doubanCrawler::startsWith(prefix, source, 2, 6));
}

void testParseElement() {
  const std::string source = R"(<div id="main" class="test"></div>)";
  doubanCrawler::Parser parser(0, source);
  doubanCrawler::Node node = parser.parseElement();
  std::string tagName = node.getNodeData().element.getTagName();
  EXPECT_EQ_CSTRING("div", tagName.c_str());
}

void testParseNode() {
  const std::string source = R"(<p>hello world</p>)";
  doubanCrawler::Parser parser(0, source);
  doubanCrawler::Node node = parser.parseNode();
  EXPECT_EQ_TRUE(node.getNodeData().isElementData());
  EXPECT_EQ_TRUE(node.getChildren().size() == 1);
  doubanCrawler::Node childrenNode = node.getChildren().at(0);
  EXPECT_EQ_TRUE(childrenNode.getNodeData().isText());
  EXPECT_EQ_CSTRING("hello world", childrenNode.getNodeData().text.c_str());
}

void testParseAttributes() {
  const std::string source = R"(<div id="main" class="test"></div>)";
  doubanCrawler::Parser parser(0, source);
  std::vector<doubanCrawler::Node> nodes = parser.parseNodes();
  doubanCrawler::Node node = nodes.at(0);
  EXPECT_EQ_TRUE(node.getNodeData().isElementData());
}

void testParse() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  doubanCrawler::Node node = doubanCrawler::parse(source);
}

void testParseSelfClosingTag(){
  std::ifstream file("source/parseSelfClosingTagTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  doubanCrawler::Node node = doubanCrawler::parse(source);
}

void testStringContains() {
  std::string substring("needle");
  std::string source("There are two needles in this haystack.");
  EXPECT_EQ_TRUE(doubanCrawler::contains(substring, source));
}

void testConsumeComment() {
  std::ifstream file("source/commentTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string emptyString;
  std::regex pattern(R"(<!--.*?-->)");
  std::string result = std::regex_replace(buffer.str(), pattern, emptyString);
  std::string commentDescriptor("<!--");
  EXPECT_EQ_TRUE(!doubanCrawler::contains(commentDescriptor, result));
}

void testHttp() {
  /* first where are we going to send it? */
  const std::string host = "stackoverflow.com";
  doubanCrawler::http_get(host);
}

void printNode(const doubanCrawler::Node &result) {
  TRACE(("tagName: %s\n", result.getNodeData().element.getTagName().c_str()));
  doubanCrawler::AttrMap attributes =
      result.getNodeData().element.getAttributes();
  for (auto const &keyValuePair : attributes) {
    TRACE(("key: %s value: %s\n", keyValuePair.first.c_str(),
           keyValuePair.second.c_str()));
  }
}

void testGetElementsByTag() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  doubanCrawler::Node node = doubanCrawler::parse(source);
  doubanCrawler::Elements elements = node.getElementsByTag(std::string("div"));
  for (auto const &result : elements) {
    printNode(result);
  }
}

void testGetElementById() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  doubanCrawler::Node node = doubanCrawler::parse(source);
  doubanCrawler::Node result = node.getElementById("main");
  printNode(result);
  EXPECT_EQ_CSTRING(
      result.getNodeData().element.getAttributes().at("class").c_str(), "test");
}

int main() {
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
  //testParseSelfClosingTag();
  PRINT_TEST_RESULT();
}
