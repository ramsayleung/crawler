//
// Created by Ramsay on 2019/9/9.
//

#include "dom.hpp"
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
