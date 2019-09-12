//
// Created by Ramsay on 2019/9/9.
//
#include "test.hpp"
#include "dom.hpp"
#include "html.hpp"

void testSplit() {
  std::string classes = "item.pic.info";
  std::set<std::string> classSet;
  String::split(classes, classSet, '.');
  EXPECT_EQ_INT(3, classSet.size());
}

void testStartsWith() {
  const std::string source = "tititoto";
  std::string prefix = "titi";
  EXPECT_EQ_INT(1, String::startsWith(prefix, source));
  prefix = "tito";
  EXPECT_EQ_INT(true, String::startsWith(prefix, source, 2, 6));
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

int main(int argc, char *argv[]) {
  testSplit();
  testStartsWith();
  testParse();
}
