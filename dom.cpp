//
// Created by Ramsay on 2019/9/9.
//

#include "dom.hpp"

#include <utility>
crawler::Nodes crawler::Node::getElementsByTag(const std::string &tagName) {
  return getElementsByPredicate([&tagName](const Node &node) -> bool {
    return node.getNodeData().element.getTagName() == tagName;
  });
}

crawler::Node crawler::Node::getElementById(const std::string &id) {
  Nodes elements = getElementsByPredicate([&id](const Node &node) -> bool {
    return node.getNodeData().element.id() == id;
  });
  assert(elements.size() == 1);
  return elements[0];
}
const crawler::Node *
crawler::Node::getFirstElementByTagName(const std::string &tagName,
                                        const crawler::Node *node) const {
  return getFirstElementByPredicate(
      [&tagName](const Node *input) -> bool {
        return input->getNodeData().element.getTagName() == tagName;
      },
      node);
}
const crawler::Nodes crawler::Node::select(const std::string &cssQuery) const {
  return crawler::Nodes();
}
std::string crawler::ElementData::clazz() {
  auto clazz = attributes.find("class");
  std::string classValue;
  if (clazz != attributes.end()) {
    return clazz->second;
  }
  return classValue;
}
std::string crawler::ElementData::id() {
  auto result = attributes.find("id");
  if (result != attributes.end()) {
    return result->second;
  } else {
    return std::string("");
  }
}
