
#ifndef DOUBANCRAWLER_DOM_H
#define DOUBANCRAWLER_DOM_H

#include "strings.hpp"
#include <array>
#include <cassert>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>
namespace crawler {
class Evaluator;
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

  Nodes select(Evaluator *evaluator);

  Nodes select(const std::string &cssQuery);

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

  /// Get elements by call `predicate(node)` using BFS
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

class TokenQueue {
public:
  explicit TokenQueue(std::string data);

  explicit TokenQueue(std::string data, size_t pos);

  /// Consume whitespace.
  void consumeWhiteSpace();

  /// Tests if the next characters is whitespace.
  bool matchesWhitespace();

  /// Tests if the next characters reaches the eof.
  bool eof();

  /// Tests if the next characters on the queue match the sequence.
  bool matches(const std::string &seq);

  /// Tests if the queue matches the sequence (as with match), and if they do
  /// removes the matched string from the queue.
  bool matchesChomp(const std::string &seq);

  /// Test if the queue matches a word character (letter or digit).
  bool matchesWords();

  /// Tests if the next characters match any of the sequences
  template <size_t N> bool matchesAny(std::array<std::string, N> seq);

  /// Tests if the next characters match any of the sequences
  template <size_t N> bool matchesAny(std::array<char, N> seq);

  /// Consume a CSS identifier (ID or class) off the queue (letter, digit, -, _)
  std::string consumeCssIdentifier();

  /// Consume a CSS element selector (tag name, but | instead of : for
  /// namespaces (or *| for wildcard namespace), to not conflict with :pseudo
  /// selects).
  std::string consumeElementSelector();

  /// Consume by predate
  template <typename Predicate>
  std::string consumeByPredicate(Predicate predicate);

  [[nodiscard]] const std::string &getData() const;

  [[nodiscard]] size_t getPos() const;

private:
  std::string data;
  size_t pos;
};

class Evaluator {
public:
  virtual bool matches(const Node &root, const Node &node) = 0;
};
class CombiningEvaluator : public Evaluator {
public:
  explicit CombiningEvaluator(std::vector<Evaluator *> evalutors);
  virtual bool matches(const crawler::Node &root, const crawler::Node &node) {
    return false;
  }

protected:
  std::vector<Evaluator *> evalutors;
};

class And final : public CombiningEvaluator {
public:
  explicit And(const std::vector<Evaluator *> &evalutors);
  bool matches(const crawler::Node &root, const crawler::Node &node) override;
};

class Or final : public CombiningEvaluator {
public:
  explicit Or(const std::vector<Evaluator *> &evalutors);
  bool matches(const crawler::Node &root, const crawler::Node &node) override;
};

class Id : public Evaluator {
public:
  explicit Id(std::string id);
  bool matches(const crawler::Node &root, const crawler::Node &node) override;

private:
  std::string id;
};
class Class : public Evaluator {
public:
  explicit Class(std::string clazz);
  bool matches(const crawler::Node &root, const crawler::Node &node) override;

private:
  std::string clazz;
};

class Tag : public Evaluator {
public:
  explicit Tag(std::string tagName);
  bool matches(const crawler::Node &root, const crawler::Node &node) override;

private:
  std::string tagName;
};

class QueryParser {
public:
  explicit QueryParser(const std::string &queryString);

  std::shared_ptr<Evaluator *> parse();

private:
  /// find elements.
  void findElements();

  /// Find element by id.
  void findById();

  /// Find element by class name.
  void findByClass();

  /// Find element by tag name.
  void findByTag();
  std::string queryString;
  TokenQueue tokenQueue;
  std::vector<Evaluator *> evals;
};

} // namespace crawler

#endif // DOUBANCRAWLER_DOM_H
