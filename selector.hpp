//
// Created by Ramsay on 2019/11/6.
//

#ifndef DOUBANCRAWLER_SELECTOR_H
#define DOUBANCRAWLER_SELECTOR_H
#include "dom.hpp"
#include <array>
#include <set>
#include <string>
#include <vector>
namespace crawler {
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
  virtual bool matches(const crawler::Node &root,
                       const crawler::Node &node) = 0;
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
  QueryParser(std::string queryString, TokenQueue tokenQueue);
  Evaluator parse();

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
#endif // DOUBANCRAWLER_SELECTOR_H
