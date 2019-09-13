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

void testSplit() {
    std::string classes = "item.pic.info";
    std::set<std::string> classSet;
    doubanCrawler::split(classes, classSet, '.');
    EXPECT_EQ_INT(3, classSet.size());
}

void testStartsWith() {
    const std::string source = "tititoto";
    std::string prefix = "titi";
    EXPECT_EQ_INT(1, doubanCrawler::startsWith(prefix, source));
    prefix = "tito";
    EXPECT_EQ_INT(true, doubanCrawler::startsWith(prefix, source, 2, 6));
}

void testParseTagName() {
    const std::string source = R"(<div id="main" class="test"></div>)";
    doubanCrawler::Parser parser(0, source);
    std::string tagName = parser.parseTagName();
    EXPECT_EQ_STRING(tagName.c_str(), "div");
}

void testParseAttributes() {
    const std::string source = R"(<div id="main" class="test"></div>)";
    doubanCrawler::Parser parser(0, source);
    std::vector<doubanCrawler::Node> nodes = parser.parseNodes();
    doubanCrawler::Node node = nodes.at(0);
    EXPECT_EQ_TRUE(node.getNodeType().isElementData());
}

void testParse() {
    const std::string source = R"(
  <html>
      <body>
          <h1>Title</h1>
          <div id="main" class="test">
              <p>Hello <em>world</em>!</p>
          </div>
      </body>
  </html>)";

    doubanCrawler::Node node = doubanCrawler::parse(source);
}

void testHttp() {
    /* first where are we going to send it? */
    const std::string host = "stackoverflow.com";
    doubanCrawler::http_get(host);
}

int main(int argc, char *argv[]) {
    testSplit();
    testStartsWith();
    testParseTagName();
    testParseAttributes();
    testParse();
    testHttp();
}
