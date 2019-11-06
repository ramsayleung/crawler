
#ifndef DOUBANCRAWLER_DOM_H
#define DOUBANCRAWLER_DOM_H

#include "strings.hpp"
#include <cassert>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>
namespace crawler {
class Node;
using AttrMap = std::map<std::string, std::string>;

using Nodes = std::vector<Node>;

class ElementData {
public:
  ElementData() = default;
  ElementData(std::string tagName, AttrMap attributes)
      : tagName(std::move(tagName)), attributes(std::move(attributes)) {}
  /// Get id of element
  std::string id();

  /// Get value of class.
  std::string clazz();

  [[nodiscard]] const std::string &getTagName() const { return tagName; }
  [[nodiscard]] const AttrMap &getAttributes() const { return attributes; }

private:
  std::string tagName;
  AttrMap attributes;
};

typedef struct NodeType {
public:
  bool isText() { return !text.empty(); }
  bool isElementData() { return !isText(); }
  ElementData element;
  std::string text;
} NodeData;

class Node {
public:
  // data common to all nodes;
  explicit Node(const std::string &str) {
    nodeData.text = str;
    ElementData emptyElement;
    nodeData.element = emptyElement;
  }
  explicit Node(const std::string &_name, const AttrMap &_attrMap,
                std::vector<Node> _children) {
    children = std::move(_children);
    nodeData.text = "";
    ElementData elementData(_name, _attrMap);
    nodeData.element = elementData;
  }

  Node(const Node &node) {
    children = node.children;
    nodeData = node.nodeData;
  }

  /// get children elements
  [[nodiscard]] std::vector<Node> getChildren() const { return children; }

  /// get node data
  [[nodiscard]] NodeData getNodeData() const { return nodeData; }

  /// Search element list by tag name by bfs(breadth first search)
  Nodes getElementsByTag(const std::string &tagName);

  /// Get element list by element id.
  Node getElementById(const std::string &id);

  const Nodes select(const std::string &cssQuery) const;

private:
  /// data common to all nodes;
  std::vector<Node> children;

  /// data specific to each node type;
  NodeData nodeData;

  const Node *getFirstElementByTagName(const std::string &tagName,
                                       const Node *node) const;

  /// Get first element by predicate.
  template <typename Predicate>
  const Node *getFirstElementByPredicate(Predicate predicate,
                                         const Node *node) const {
    if (node->getNodeData().isElementData() && predicate(node)) {
      return node;
    } else {
      for (auto &childrenNode : node->getChildren()) {
        const Node *found =
            getFirstElementByPredicate(predicate, &childrenNode);
        if (found != nullptr) {
          return found;
        }
      }
    }
    return nullptr;
  }

  /// Get elements by call `predicate(node)` using DFS
  template <class Predicate> Nodes getElementsByPredicate(Predicate predicate) {
    Nodes elementList;
    std::queue<Node> queue;
    Node root = *this;
    queue.push(root);
    while (!queue.empty()) {
      Node tempNode = queue.front();
      queue.pop();
      if (tempNode.getNodeData().isElementData() && predicate(tempNode)) {
        elementList.emplace_back(tempNode);
      }
      if (!tempNode.getChildren().empty()) {
        for (auto const &childrenNode : tempNode.getChildren()) {
          queue.push(childrenNode);
        }
      }
    }
    return elementList;
  }
};

} // namespace crawler

#endif // DOUBANCRAWLER_DOM_H
