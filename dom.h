
#ifndef DOUBANCRAWLER_DOM_H
#define DOUBANCRAWLER_DOM_H

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "strings.h"
namespace doubanCrawler {
using AttrMap = std::map<std::string, std::string>;

class ElementData {
 public:
  ElementData() = default;
  ElementData(std::string tagName, AttrMap attributes)
      : tagName(std::move(tagName)), attributes(std::move(attributes)) {}
  std::string id() {
    auto result = attributes.find("id");
    if (result != attributes.end()) {
      return result->second;
    } else {
      return std::string("");
    }
  }
  std::set<std::string> classes() {
    auto clazz = attributes.find("class");
    std::set<std::string> classElement;
    if (clazz != attributes.end()) {
      std::string result = clazz->second;
      doubanCrawler::split(result, classElement, '.');
    }
    return classElement;
  }

 private:
  std::string tagName;
  AttrMap attributes;
};

typedef struct NodeType {
  ElementData element;
  std::string text;
} NodeType;

class Node {
 public:
  // data common to all nodes;
  explicit Node(const std::string &str) {
    nodeType.text = str;
    ElementData emptyElement;
    nodeType.element = emptyElement;
  };
  explicit Node(const std::string &_name, const AttrMap &_attrMap, std::vector<Node> _children) {
    children = std::move(_children);
    nodeType.text = nullptr;
    ElementData elementData(_name, _attrMap);
    nodeType.element = elementData;
  }

  Node(const Node &node) {
    children = node.children;
    nodeType = node.nodeType;
  }

 private:
  // data common to all nodes;
  std::vector<Node> children;
  // data specific to each node type;
  NodeType nodeType;
};
}

#endif // DOUBANCRAWLER_DOM_H
