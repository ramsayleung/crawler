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

void testEndsWith() {
  const std::string source = "tititoto";
  std::string suffix = "toto";
  ASSERT_TRUE(crawler::endsWith(suffix, source));
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
  crawler::Node node = crawler::parse(buffer.str());
  printNode(node);
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
  crawler::Nodes nodes = node.select("div.child");
  ASSERT_UNSIGNED_LONG_EQ(nodes.size(), 1UL);
  crawler::Node nodeWithChildClass = nodes.front();
  ASSERT_CSTRING_EQ(nodeWithChildClass.getElementData().id().c_str(), "child");

  nodes = node.select("div.parent > div.child");
  ASSERT_UNSIGNED_LONG_EQ(nodes.size(), 1UL);
  crawler::Node childNodeWithChildClass = nodes.front();
  ASSERT_CSTRING_EQ(childNodeWithChildClass.getElementData().id().c_str(),
                    "child");

  crawler::Nodes anotherNodes = node.select("div#main > div");
  ASSERT_TRUE(anotherNodes.size() == 1);
  crawler::Node divNode = anotherNodes.front();
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

void testIndexOf() {
  const std::string source = "Helloworld";
  const std::string subString = "world";
  size_t index = crawler::indexOf(subString, source);
  ASSERT_UNSIGNED_LONG_EQ(5UL, index);
}

void testConsumeToAny() {
  crawler::TokenQueue tokenQueue("TITLE=foo");
  std::string title = tokenQueue.consumeToAny(crawler::QueryParser::ATTRIBUTES);
  ASSERT_CSTRING_EQ(title.c_str(), "TITLE");
}

void testSelectByAttributeKey() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("[method]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  crawler::Node form = nodes.front();
  ASSERT_CSTRING_EQ("form", form.getElementData().getTagName().c_str());
}

void testSelectByAttributeKeyStartWithPrefix() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select("[^act]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  crawler::Node form = nodes.front();
  ASSERT_CSTRING_EQ("form", form.getElementData().getTagName().c_str());
}

void testNormalize() {
  std::string input("Abc");
  ASSERT_CSTRING_EQ("abc", crawler::normalize(input).c_str());
}

void testSelectByAttributeWithValue() {
  std::ifstream file("source/parseTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  crawler::Node node = crawler::parse(source);
  crawler::Nodes nodes = node.select(R"([for="inp-query"])");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  crawler::Node label = nodes.front();
  ASSERT_CSTRING_EQ("label", label.getElementData().getTagName().c_str());

  nodes = node.select(R"(.test > div[class=parent])");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  crawler::Node div = nodes.front();
  ASSERT_CSTRING_EQ("parent", div.getElementData().id().c_str());
}

void testSelectByAttributeWithValueNot() {
  crawler::Node node =
      crawler::parse(R"(<div name="ramsay"></div><div name="fancy"></div>)");
  crawler::Nodes nodes = node.select("div[name!=fancy]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  ASSERT_CSTRING_EQ(
      "ramsay", nodes.front().getElementData().getValueByKey("name").c_str());
}

void testSelectByAttributeValueStartWithPrefix() {
  crawler::Node node = crawler::parse(
      R"(<div name="ramsay"></div><div name="fancy" id="test"></div>)");
  crawler::Nodes nodes = node.select("[name^=fancy]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  ASSERT_CSTRING_EQ("test", nodes.front().getElementData().id().c_str());
}

void testSelectByAttributeValueEndWithSuffix() {
  crawler::Node node = crawler::parse(
      R"(<div name="ramsay" id="foo"></div><div name="fancy" id="test"></div>)");
  crawler::Nodes nodes = node.select("[name$=ay]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  ASSERT_CSTRING_EQ("foo", nodes.front().getElementData().id().c_str());
}

void testSelectByAttributeValueContainsSubString() {
  crawler::Node node = crawler::parse(
      R"(<div name="ramsay" id="foo"></div><div name="fancy" id="test"></div>)");
  crawler::Nodes nodes = node.select("[name*=ms]");
  ASSERT_UNSIGNED_LONG_EQ(1UL, nodes.size());
  ASSERT_CSTRING_EQ("foo", nodes.front().getElementData().id().c_str());
}

void testContainsIgnoreCase() {
  ASSERT_TRUE(crawler::containsIgnoreCase("Doctype", "DOCTYPE"));
}

void testParseDoctype() {
  std::ifstream file("source/commentTest.html");
  std::stringstream buffer;
  buffer << file.rdbuf();
  crawler::Node node = crawler::parse(buffer.str());
  printNode(node);
}

int main() {
  testParseDoctype();
  testContainsIgnoreCase();
  testSelectByAttributeValueContainsSubString();
  testSelectByAttributeValueEndWithSuffix();
  testSelectByAttributeValueStartWithPrefix();
  testSelectByAttributeWithValueNot();
  testNormalize();
  testSelectByAttributeWithValue();
  testSelectByAttributeKeyStartWithPrefix();
  testSelectByAttributeKey();
  testConsumeToAny();
  testIndexOf();
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
  testEndsWith();
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
