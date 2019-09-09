
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
      String::split(result, classElement, '.');
    }
    return classElement;
  }

 private:
  std::string tagName;
  AttrMap attributes;
};
typedef enum NodeTypeType { ELEMENT, TEXT } NodeTypeType;
typedef union NodeTypeValue {
  ElementData element;
  std::string text;
  ~NodeTypeValue() {}
  NodeTypeValue() {}
  NodeTypeValue(const NodeTypeValue &rhs) {
    element = rhs.element;
    text = rhs.text;
  }
  NodeTypeValue &operator=(const NodeTypeValue &rhs) {
    element = rhs.element;
    text = rhs.text;
    return *this;
  }
} NodeTypeValue;

typedef struct NodeType {
  NodeType() = default;
  explicit NodeType(NodeTypeType _type, const std::string &basicString) {
    type = _type;
    value.text = basicString;
  }
  explicit NodeType(NodeTypeType _type, const std::string &_name, const AttrMap &_attribute) {
    type = _type;
    value.element = ElementData(_name, _attribute);
  }
  NodeType(const NodeType &nodeType) {
    type = nodeType.type;
    value = nodeType.value;
  }
  NodeTypeType type;
  NodeTypeValue value;
} NodeType;

class Node {
 public:
  // data common to all nodes;
  explicit Node(const std::string &str) : nodeType(TEXT, str) {};
  explicit Node(const std::string &_name, const AttrMap &_attrMap, std::vector<Node> _children) :
      children(std::move(_children)),
      nodeType(ELEMENT,
               _name,
               _attrMap) {
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

#endif DOUBANCRAWLER_DOM_H
