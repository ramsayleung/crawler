
#ifndef DOUBANCRAWLER_DOM_H
#define DOUBANCRAWLER_DOM_H

#include "strings.hpp"
#include <array>
#include <cassert>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>
namespace crawler {
class Evaluator;
class Node;
using AttrMap = std::map<std::string, std::string>;
using Nodes = std::vector<Node>;

/// escape char for chomp balanced.
inline const static char ESC = '\\';

class ElementData {
public:
  ElementData() = default;
  ElementData(std::string tagName, AttrMap attributes)
      : tagName(std::move(tagName)), attributes(std::move(attributes)) {}
  /// Get id of element
  [[nodiscard]] std::string id() const;

  /// Get value of class.
  [[nodiscard]] std::string clazz() const;

  [[nodiscard]] const std::string &getTagName() const { return tagName; }

  [[nodiscard]] const AttrMap &getAttributes() const { return attributes; }

  [[nodiscard]] bool containsAttribute(const std::string &key) const;

  [[nodiscard]] const std::string getValueByKey(const std::string &key) const;

private:
  /// The tag name of current node; eg <div class="test">, tagName = "div"

  std::string tagName;
  /// The attributes of current node; eg <div class="test">, attribute = { class
  /// = "div" }
  AttrMap attributes;
};

using NodeData = std::variant<ElementData, std::string>;

enum class NodeType { Element, Text };

class Node {
public:
  // data common to all nodes;
  explicit Node(const std::string &str)
      : nodeType(NodeType::Text), nodeData(str), parent(nullptr) {}

  // data common to all nodes;
  explicit Node(const std::string &str, std::shared_ptr<Node> _parent)
      : nodeType(NodeType::Text), nodeData(str), parent(std::move(_parent)) {}

  explicit Node(const std::string &_name, const AttrMap &_attrMap,
                std::vector<Node> _children,
                const std::shared_ptr<Node> &_parent) {
    children = std::move(_children);
    parent = _parent;
    ElementData elementData(_name, _attrMap);
    nodeType = NodeType::Element;
    nodeData = elementData;
  }

  /// get children elements
  [[nodiscard]] std::vector<Node> getChildren() const { return children; }

  /// Search element list by tag name by bfs(breadth first search)
  Nodes getElementsByTag(const std::string &tagName);

  /// Get element list by element id.
  Node getElementById(const std::string &id);

  /// Query note by css query syntax.
  Nodes select(Evaluator *evaluator);

  /// Query note by css query syntax.
  Nodes select(const std::string &cssQuery);

  /// Get element data of current node, throw exception if it's not a element
  /// type.
  [[nodiscard]] const ElementData &getElementData() const;

  /// Get text of current node, throw exception if it's not a text type.
  [[nodiscard]] const std::string &getText() const;

  [[nodiscard]] const std::shared_ptr<Node> &getParent() const;

  /// If current node is a element type.
  [[nodiscard]] bool isElement() const;

  /// If current node is a text type.
  [[nodiscard]] bool isText() const;

private:
  /// data common to all nodes;
  std::vector<Node> children;

  /// Node type
  NodeType nodeType;

  /// Node data, element or text
  NodeData nodeData;

  std::shared_ptr<Node> parent;

  /// Get elements by call `predicate(node)` using BFS
  template <class Predicate> Nodes getElementsByPredicate(Predicate predicate) {
    Nodes elementList;
    std::queue<Node> queue;
    Node root = *this;
    queue.push(root);
    while (!queue.empty()) {
      Node tempNode = queue.front();
      queue.pop();
      if (tempNode.isElement() && predicate(tempNode)) {
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
  bool consumeWhiteSpace();

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
  template <size_t N> bool matchesAny(std::array<std::string, N> seq) {
    return std::any_of(seq.cbegin(), seq.cend(),
                       [&](const std::string &s) { return matches(s); });
  }

  /// Tests if the next characters match any of the sequences
  template <size_t N> inline bool matchesAny(std::array<char, N> seq) {
    if (eof()) {
      return false;
    }
    return std::any_of(seq.cbegin(), seq.cend(),
                       [this](const char c) { return data.c_str()[pos] == c; });
  }

  /// Consume to the first sequence provided
  template <size_t N>
  inline std::string consumeToAny(std::array<std::string, N> seq) {
    size_t start = pos;
    while (!eof() && !matchesAny(seq)) {
      pos++;
    }
    size_t length = pos - start;
    return data.substr(start, length);
  }

  /// Consume a CSS identifier (ID or class) off the queue (letter, digit, -, _)
  std::string consumeCssIdentifier();

  /// Consume a CSS element selector (tag name, but | instead of : for
  /// namespaces (or *| for wildcard namespace), to not conflict with :pseudo
  /// selects).
  std::string consumeElementSelector();

  /// Consume a sub query
  std::string consumeSubQuery();

  /// Pulls a balanced string off the queue. E.g. if queue is "(one (two) three)
  /// four", (,) will return "one (two) three", and leave " four" on the queue.
  /// Unbalanced openers and closers can be quoted (with ' or ") or escaped
  /// (with \). Those escapes will be left in the returned string, which is
  /// suitable for regexes (where we need to preserve the escape), but
  /// unsuitable for contains text strings; use unescape for that.
  std::string chompBalanced(char open, char close);

  /// Consume one character off queue, eg move pos advanced one step.
  char consume();

  /// Consume by predate
  template <typename Predicate>
  std::string consumeByPredicate(Predicate predicate);

  /// Consume and return whatever is left on the queue
  [[nodiscard]] std::string remainder();

  [[nodiscard]] const std::string &getData() const;

  [[nodiscard]] size_t getPos() const;

private:
  std::string data;
  size_t pos;
};

/// Evaluator that an element need to match the selector
/// Such as, Id Evaluator means the selector will compare the id of an element
/// with the given id.
class Evaluator {
public:
  /// Derived class need to implement this function to show how to match.
  virtual bool matches(const Node &element) = 0;
};

/// Combining Evaluator.
class CombiningEvaluator : public Evaluator {
public:
  explicit CombiningEvaluator(std::vector<Evaluator *> evalutors);
  virtual bool matches(const Node &element) { return false; }

protected:
  std::vector<Evaluator *> evalutors;
};

/// Structural Evaluator
class StructuralEvaluator : public Evaluator {
public:
  explicit StructuralEvaluator(std::shared_ptr<Evaluator *> _eval)
      : evaluator(std::move(_eval)) {}
  virtual bool matches(const Node &element) { return false; }

protected:
  std::shared_ptr<Evaluator *> evaluator;
};

class Parent : public StructuralEvaluator {
public:
  explicit Parent(std::shared_ptr<Evaluator *> _evaluator);
  bool matches(const Node &element) override;
};

class ImmediateParent : public StructuralEvaluator {
public:
  explicit ImmediateParent(std::shared_ptr<Evaluator *> _evaluator);
  bool matches(const Node &element) override;
};

class And final : public CombiningEvaluator {
public:
  explicit And(const std::vector<Evaluator *> &evalutors);
  bool matches(const Node &element) override;
};

class Or final : public CombiningEvaluator {
public:
  explicit Or(const std::vector<Evaluator *> &evalutors);
  bool matches(const Node &element) override;
};

/// AttributeKeyValuePair Evaluator, select by attribute key or/and value.
class AttributeKeyValuePair : public Evaluator {
public:
  AttributeKeyValuePair(const std::string &_key, const std::string &_value);
  bool matches(const Node &element) override;

protected:
  std::string key;
  std::string value;
};

/// AttributeWithValue Evaluator, means that select by value matches the given
/// `value`.
class AttributeWithValue : public AttributeKeyValuePair {
public:
  AttributeWithValue(const std::string &key, const std::string &value);
  bool matches(const Node &element) override;
};

/// AttributeWithValueNot Evaluator, means select by value doesn't start with
/// given `value`
class AttributeWithValueNot : public AttributeKeyValuePair {
public:
  AttributeWithValueNot(const std::string &key, const std::string &value);
  bool matches(const Node &root) override;
};

/// Evaluator for attribute key/value matching (value prefix)
class AttributeValueStartWithPrefix : public AttributeKeyValuePair {
public:
  AttributeValueStartWithPrefix(const std::string &key,
                                const std::string &value);
  bool matches(const Node &root) override;
};

/// Evaluator for attribute key/value matching (value suffix)
class AttributeValueEndWithSuffix : public AttributeKeyValuePair {
public:
  AttributeValueEndWithSuffix(const std::string &key, const std::string &value);
  bool matches(const Node &root) override;
};

/// Evaluator for attribute key/value matching (value containing)

class AttributeValueContainWithSubstring : public AttributeKeyValuePair {
public:
  AttributeValueContainWithSubstring(const std::string &key,
                                     const std::string &value);
  bool matches(const Node &root) override;
};

/// Id Evaluator, means that selector will compare the id of an element with the
/// given one.
class Id : public Evaluator {
public:
  explicit Id(std::string id);
  bool matches(const Node &element) override;

private:
  std::string id;
};

/// Class Evaluator, means that selector will compare the class name of an
/// element with the given one.
class Class : public Evaluator {
public:
  explicit Class(std::string clazz);
  bool matches(const Node &element) override;

private:
  std::string clazz;
};

/// Tag Evaluator, means that selector will compare the tag name of an element
/// with the given one.
class Tag : public Evaluator {
public:
  explicit Tag(std::string tagName);
  bool matches(const Node &element) override;

private:
  std::string tagName;
};

/// Evaluator for attribute name matching
class Attribute : public Evaluator {
public:
  explicit Attribute(std::string key);
  bool matches(const Node &element) override;

private:
  std::string key;
};

/// Evaluator for attribute name prefix matching
class AttributeKeyStartWithPrefix : public Evaluator {
public:
  explicit AttributeKeyStartWithPrefix(std::string keyPrefix);
  bool matches(const Node &element) override;

private:
  std::string keyPrefix;
};

class QueryParser {
public:
  explicit QueryParser(const std::string &queryString);

  std::shared_ptr<Evaluator *> parse();

  static std::shared_ptr<Evaluator *> parse(const std::string &cssQuery);

  inline static const std::array<std::string, 5> COMBINATORS = {",", ">", "+",
                                                                "~", " "};
  inline static const std::array<std::string, 6> ATTRIBUTES = {
      "=", "!=", "^=", "$=", "*=", "~="};

private:
  /// find elements.
  void findElements();

  /// Find element by id.
  void findById();

  /// Find element by class name.
  void findByClass();

  /// Find element by tag name.
  void findByTag();

  /// Find element by attribute.
  void findByAttribute();

  /// Combine child selector.
  void combinator(char combinator);

  /// member variables.

  std::string queryString;

  TokenQueue tokenQueue;

  std::vector<Evaluator *> evals;
};

} // namespace crawler
#endif // DOUBANCRAWLER_DOM_H
