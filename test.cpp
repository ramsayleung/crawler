//
// Created by Ramsay on 2019/9/9.
//
#include "test.hpp"
/* read, write, close */
#include <unistd.h>
#include "dom.hpp"
#include "html.hpp"
#include "http.hpp"
#include "utils.hpp"

#include <fstream>
#include <regex>
#include <streambuf>
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
    std::string tagName = node.getNodeType().element.getTagName();
    EXPECT_EQ_CSTRING("div", tagName.c_str());
}

void testParseNode() {
    const std::string source = R"(<p>hello world</p>)";
    doubanCrawler::Parser parser(0, source);
    doubanCrawler::Node node = parser.parseNode();
    EXPECT_EQ_TRUE(node.getNodeType().isElementData());
    EXPECT_EQ_TRUE(node.getChildren().size() == 1);
    doubanCrawler::Node childrenNode = node.getChildren().at(0);
    EXPECT_EQ_TRUE(childrenNode.getNodeType().isText());
    EXPECT_EQ_CSTRING("hello world", childrenNode.getNodeType().text.c_str());
}

void testParseAttributes() {
    const std::string source = R"(<div id="main" class="test"></div>)";
    doubanCrawler::Parser parser(0, source);
    std::vector<doubanCrawler::Node> nodes = parser.parseNodes();
    doubanCrawler::Node node = nodes.at(0);
    EXPECT_EQ_TRUE(node.getNodeType().isElementData());
}

void testParse() {
    std::ifstream file("source/parseTest.html");
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
    std::string emptyString("");
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

int main(int argc, char *argv[]) {
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
    PRINT_TEST_RESULT();
}
