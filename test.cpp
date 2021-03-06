//
// Created by Ramsay on 2019/9/9.
//
#include "test.hpp"
#include "dom.hpp"
#include "html.hpp"
#include "http.hpp"
#include "json.hpp"
#include "utils.hpp"

#include <fstream>
#include <regex>
#include <string>

/// Html Start
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

/// Html End

/// JSON Start
void testJsonParseNull() {
  std::string json(" null");
  crawler::JsonParser parser(json);
  crawler::JsonValue jsonValue = parser.parse();
  ASSERT_TRUE(jsonValue.getType() == crawler::JsonType::_NULL);
  ASSERT_TRUE(nullptr == jsonValue.getNull());
}

void testJsonParseBoolean() {
  crawler::JsonParser trueParser(" true");
  crawler::JsonValue jsonValue = trueParser.parse();
  ASSERT_TRUE(jsonValue.getType() == crawler::JsonType::BOOLEAN);
  ASSERT_TRUE(jsonValue.getBoolean());

  crawler::JsonParser falseParser("false");
  crawler::JsonValue jsonValue1 = falseParser.parse();
  ASSERT_TRUE(jsonValue1.getType() == crawler::JsonType::BOOLEAN);
  ASSERT_FALSE(jsonValue1.getBoolean());
}

void testJsonParseError() {
  crawler::JsonParser trueParser(" true xx");
  crawler::JsonParser numberParser(" 0 12");
  try {
    trueParser.parse();

    // it should be unreachable, otherwise, it should failed.
    ASSERT_TRUE(0);
  } catch (const std::runtime_error &error) {
    ASSERT_CSTRING_EQ("PARSE_ROOT_NOT_SINGULAR", error.what());
  }
  try {
    numberParser.parse();
    ASSERT_TRUE(0);
  } catch (const std::runtime_error &error) {
    ASSERT_CSTRING_EQ("PARSE_ROOT_NOT_SINGULAR", error.what());
  }
}

void testJsonParseNumber(double expect, const std::string &json) {
  crawler::JsonParser trueParser(json);
  crawler::JsonValue jsonValue = trueParser.parse();
  ASSERT_TRUE(jsonValue.getType() == crawler::JsonType::NUMBER);
  ASSERT_DOUBLE_EQ(expect, jsonValue.getNumber());
}

void testJsonParseError(const std::string &error, const std::string &json) {

  crawler::JsonParser parser(json);

  try {
    parser.parse();
    // unreachable line.
    ASSERT_TRUE(0);
  } catch (const std::runtime_error &ex) {
    ASSERT_CSTRING_EQ(error.c_str(), ex.what());
  }
}

void testJsonParseNumberError() {
  testJsonParseError("PARSE_INVALID_VALUE", "+0");
  testJsonParseError("PARSE_INVALID_VALUE", "+1");
  /* at least one digit before '.' */
  testJsonParseError("PARSE_INVALID_VALUE", ".123");
  /* at least one digit after '.' */
  testJsonParseError("PARSE_INVALID_VALUE", "1.");
  testJsonParseError("PARSE_INVALID_VALUE", "INF");
  testJsonParseError("PARSE_INVALID_VALUE", "inf");
  testJsonParseError("PARSE_INVALID_VALUE", "NAN");
  testJsonParseError("PARSE_INVALID_VALUE", "nan");
}

void testJsonParseNumber() {
  testJsonParseNumber(0.0, "0");
  testJsonParseNumber(0.0, "-0");
  testJsonParseNumber(0.0, "-0.0");
  testJsonParseNumber(1.0, "1");
  testJsonParseNumber(-1.0, "-1");
  testJsonParseNumber(1.5, "1.5");
  testJsonParseNumber(-1.5, "-1.5");
  testJsonParseNumber(3.1416, "3.1416");
  testJsonParseNumber(1E10, "1E10");
  testJsonParseNumber(1e10, "1e10");
  testJsonParseNumber(1E+10, "1E+10");
  testJsonParseNumber(1E-10, "1E-10");
  testJsonParseNumber(-1E10, "-1E10");
  testJsonParseNumber(-1e10, "-1e10");
  testJsonParseNumber(-1E+10, "-1E+10");
  testJsonParseNumber(-1E-10, "-1E-10");
  testJsonParseNumber(1.234E+10, "1.234E+10");
  testJsonParseNumber(1.234E-10, "1.234E-10");
}

void testJsonParseString() {
  crawler::JsonParser stringParser("\"Hello\"");
  crawler::JsonValue jsonValue = stringParser.parse();
  ASSERT_TRUE(jsonValue.getType() == crawler::JsonType::STRING);
  ASSERT_CSTRING_EQ(jsonValue.getString().c_str(), "Hello");

  crawler::JsonParser chineseParser("\"你好, 世界\"");
  crawler::JsonValue chineseJsonValue = chineseParser.parse();
  ASSERT_TRUE(chineseJsonValue.getType() == crawler::JsonType::STRING);
  ASSERT_CSTRING_EQ(chineseJsonValue.getString().c_str(), "你好, 世界");
}

void testJsonParseEscapeString() {
  crawler::JsonParser parser("\"\\b\"");
  crawler::JsonValue value = parser.parse();
  ASSERT_TRUE(value.getType() == crawler::JsonType::STRING);
  ASSERT_CSTRING_EQ(value.getString().c_str(), "\b");
  testJsonParseError("PARSE_INVALID_STRING_ESCAPE", "\"\\v\"");
}

void testJsonParseArray() {
  crawler::JsonParser arrayParser("[\"hello\"]");
  crawler::JsonValue value = arrayParser.parse();
  ASSERT_TRUE(value.getType() == crawler::JsonType::ARRAY);
  ASSERT_TRUE(value.getArray().front().getType() == crawler::JsonType::STRING);

  crawler::JsonParser arrayParser2("[ null , false , true , 123 , \"abc\" ]");
  value = arrayParser2.parse();
  ASSERT_TRUE(value.getType() == crawler::JsonType::ARRAY);
  ASSERT_TRUE(value.getArray().size() == 5);
  crawler::JsonArray array = value.getArray();
  ASSERT_TRUE(array[0].getType() == crawler::JsonType::_NULL);
  ASSERT_TRUE(array[1].getType() == crawler::JsonType::BOOLEAN);
  ASSERT_TRUE(array[2].getType() == crawler::JsonType::BOOLEAN);
  ASSERT_TRUE(array[3].getType() == crawler::JsonType::NUMBER);
  ASSERT_TRUE(array[4].getType() == crawler::JsonType::STRING);

  crawler::JsonParser arrayParser3(
      "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
  value = arrayParser3.parse();
  array = value.getArray();
  crawler::JsonArray subArray = array[1].getArray();
  ASSERT_TRUE(array[0].getType() == crawler::JsonType::ARRAY);
  ASSERT_TRUE(array[1].getType() == crawler::JsonType::ARRAY);
  ASSERT_TRUE(subArray[0].getType() == crawler::JsonType::NUMBER);
  ASSERT_TRUE(subArray.size() == 1);
  ASSERT_TRUE(array[2].getType() == crawler::JsonType::ARRAY);
  subArray = array[2].getArray();
  ASSERT_TRUE(subArray.size() == 2);
  ASSERT_TRUE(subArray[0].getType() == crawler::JsonType::NUMBER);
  ASSERT_TRUE(array[3].getType() == crawler::JsonType::ARRAY);
  subArray = array[3].getArray();
  ASSERT_TRUE(subArray.size() == 3);
  ASSERT_TRUE(subArray[0].getType() == crawler::JsonType::NUMBER);
}

void testJsonParseObject() {
  crawler::JsonParser objectParser("{\"number\": 2}");
  crawler::JsonValue value = objectParser.parse();
  ASSERT_TRUE(value.getType() == crawler::JsonType::OBJECT);
  ASSERT_TRUE(value.getObject().size() == 1);
}

void testJsonParseObjectError() {
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{1:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{true:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{false:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{null:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{[]:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{{}:1,");
  testJsonParseError("PARSE_OBJECT_MISS_KEY", "{\"a\":1,");

  testJsonParseError("PARSE_OBJECT_MISS_COLON", "{\"a\"}");
  testJsonParseError("PARSE_OBJECT_MISS_COLON", "{\"a\",\"b\"}");

  testJsonParseError("PARSE_MISS_COMMA_OR_CURLY_BRACKET", "{\"a\":1");
  testJsonParseError("PARSE_MISS_COMMA_OR_CURLY_BRACKET", "{\"a\":1]");
  testJsonParseError("PARSE_MISS_COMMA_OR_CURLY_BRACKET", "{\"a\":1 \"b\"");
  testJsonParseError("PARSE_MISS_COMMA_OR_CURLY_BRACKET", "{\"a\":{}");
}
/// JSON End

int main() {
  testJsonParseObjectError();
  testJsonParseObject();
  testJsonParseArray();
  testJsonParseEscapeString();
  testJsonParseString();
  testJsonParseNumber();
  testJsonParseNumberError();
  testJsonParseError();
  testJsonParseBoolean();
  testJsonParseNull();
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
