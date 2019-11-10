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

void printNode(const crawler::Node &result) {
  TRACE(("tagName: %s\n", result.getElementData().getTagName().c_str()));
  crawler::AttrMap attributes = result.getElementData().getAttributes();
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
  std::string tagName = node.getElementData().getTagName();
  ASSERT_CSTRING_EQ("div", tagName.c_str());
}

void testParseNode() {
  const std::string source = R"(<p>hello world</p>)";
  crawler::Parser parser(0, source);
  crawler::Node node = parser.parseNode();
  ASSERT_TRUE(node.isElement());
  ASSERT_TRUE(node.getChildren().size() == 1);
  crawler::Node childrenNode = node.getChildren().at(0);
  ASSERT_TRUE(childrenNode.isText());
  ASSERT_CSTRING_EQ("hello world", childrenNode.getText().c_str());
}

void testParseAttributes() {
  const std::string source = R"(<div id="main" class="test"></div>)";
  crawler::Parser parser(0, source);
  std::vector<crawler::Node> nodes = parser.parseNodes();
  crawler::Node node = nodes.at(0);
  ASSERT_TRUE(node.isElement());
}

void testParse() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("#main");
  ASSERT_TRUE(nodes.size() == 1);
  crawler::Node mainNode = nodes.at(0);
  ASSERT_CSTRING_EQ(mainNode.getElementData().clazz().c_str(), "test");
}

void testParseSelfClosingTag() {
  std::ifstream file("source/parseSelfClosingTagTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes elements = node.getElementsByTag(std::string("img"));
  auto const element = elements[0];
  const crawler::AttrMap attributes = element.getElementData().getAttributes();
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
  ASSERT_CSTRING_EQ(result.getElementData().getAttributes().at("class").c_str(),
                    "test");
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

  crawler::TokenQueue idQueue("main > div");
  ASSERT_CSTRING_EQ("main", idQueue.consumeCssIdentifier().c_str());
}

void testParserParse() {
  crawler::QueryParser tagParser("div");
  std::shared_ptr<crawler::Evaluator *> pEval = tagParser.parse();
  // just acts like Java `instanceof` keyword.
  auto *tag = dynamic_cast<crawler::Tag *>(*pEval);
  ASSERT_TRUE(tag != nullptr);

  crawler::QueryParser idParser("#id");
  pEval = idParser.parse();
  auto *idEval = dynamic_cast<crawler::Id *>(*pEval);
  ASSERT_TRUE(idEval != nullptr);

  crawler::QueryParser classParser(".class");
  pEval = classParser.parse();
  auto *classEval = dynamic_cast<crawler::Class *>(*pEval);
  ASSERT_TRUE(classEval != nullptr);
}

void testSelect() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("div");
  for (auto const &result : nodes) {
    ASSERT_CSTRING_EQ(result.getElementData().getTagName().c_str(), "div");
  }
  nodes = node.select("#main");
  ASSERT_TRUE(nodes.size() == 1);
  crawler::Node mainNode = nodes.front();
  ASSERT_CSTRING_EQ(mainNode.getElementData().getTagName().c_str(), "div");
  ASSERT_CSTRING_EQ(mainNode.getElementData().clazz().c_str(), "test");
}

void testCombinatorSelect() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("div#main > div");
  ASSERT_TRUE(nodes.size() == 1);
  crawler::Node divNode = nodes.front();
  ASSERT_CSTRING_EQ(divNode.getElementData().id().c_str(), "parent");
}

void testChompBalanced() {
  crawler::TokenQueue tokenQueue("[href]");
  const char *href = tokenQueue.chompBalanced('[', ']').c_str();
  ASSERT_CSTRING_EQ(href, "href");

  crawler::TokenQueue queue("(one (two) three) four");
  ASSERT_CSTRING_EQ("one (two) three", queue.chompBalanced('(', ')').c_str());
}

void testConsumeSubQuery() {
  crawler::TokenQueue tokenQueue("ol, ol > li + li");
  const std::string subQuery = tokenQueue.consumeSubQuery();
  ASSERT_TRUE(subQuery == "ol");
}

void testParseParentNode() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("#child");
  ASSERT_TRUE(nodes.size() == 1);
  auto parentPtr = nodes.at(0).getParent();
  ASSERT_TRUE(parentPtr != nullptr);
  ASSERT_CSTRING_EQ(parentPtr->getElementData().id().c_str(), "parent");
}

int main() {
  testCombinatorSelect();
  testConsumeSubQuery();
  testParseParentNode();
  testChompBalanced();
  testSelect();
  testParserParse();
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
  testConsumeComment();

  // testHttp();
  PRINT_TEST_RESULT();
}
