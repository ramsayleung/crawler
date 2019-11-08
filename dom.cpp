//
// Created by Ramsay on 2019/9/9.
//

#include "dom.hpp"

#include <algorithm>
#include <utility>

crawler::Nodes crawler::Node::getElementsByTag(const std::string &tagName) {
  return getElementsByPredicate([&tagName](const Node &node) -> bool {
    return node.getElementData().getTagName() == tagName;
  });
}

crawler::Node crawler::Node::getElementById(const std::string &id) {
  Nodes elements = getElementsByPredicate([&id](const Node &node) -> bool {
    return node.getElementData().id() == id;
  });
  assert(elements.size() == 1);
  return elements[0];
}
crawler::Nodes crawler::Node::select(Evaluator *evaluator) {
  return getElementsByPredicate([&evaluator, this](const Node &node) -> bool {
    return evaluator->matches(*this, node);
  });
}

crawler::Nodes crawler::Node::select(const std::string &cssQuery) {
  crawler::QueryParser cssParser(cssQuery);
  return select(*cssParser.parse());
}
bool crawler::Node::isElement() const { return nodeType == NodeType ::Element; }

bool crawler::Node::isText() const { return nodeType == NodeType ::Text; }

const crawler::ElementData &crawler::Node::getElementData() const {
  return std::get<ElementData>(nodeData);
}
const std::string &crawler::Node::getText() const {
  return std::get<std::string>(nodeData);
}

std::string crawler::ElementData::clazz() const {
  auto clazz = attributes.find("class");
  std::string classValue;
  if (clazz != attributes.end()) {
    return clazz->second;
  }
  return classValue;
}
std::string crawler::ElementData::id() const {
  auto result = attributes.find("id");
  if (result != attributes.end()) {
    return result->second;
  } else {
    return std::string("");
  }
}

crawler::TokenQueue::TokenQueue(std::string data, size_t pos)
    : data(std::move(data)), pos(pos) {}

void crawler::TokenQueue::consumeWhiteSpace() {
  while (matchesWhitespace()) {
    pos++;
  }
}

bool crawler::TokenQueue::matchesWhitespace() {
  return !eof() && isspace(data.c_str()[pos]);
}

bool crawler::TokenQueue::eof() { return pos >= data.size(); }

bool crawler::TokenQueue::matches(const std::string &seq) {
  const std::string source = data.substr(pos, seq.size());
  return source == seq;
}

bool crawler::TokenQueue::matchesChomp(const std::string &seq) {
  if (matches(seq)) {
    pos += seq.size();
    return true;
  } else {
    return false;
  }
}
crawler::TokenQueue::TokenQueue(std::string data)
    : data(std::move(data)), pos(0) {}

const std::string &crawler::TokenQueue::getData() const { return data; }

size_t crawler::TokenQueue::getPos() const { return pos; }
std::string crawler::TokenQueue::consumeCssIdentifier() {
  return consumeByPredicate([this]() -> bool {
    return matchesAny(std::array<char, 2>({'-', '_'}));
  });
}

template <typename Predicate>
std::string crawler::TokenQueue::consumeByPredicate(Predicate predicate) {
  size_t start = pos;
  while (!eof() && (matchesWords() || predicate())) {
    pos++;
  }
  return data.substr(start, pos);
}

std::string crawler::TokenQueue::consumeElementSelector() {
  return consumeByPredicate([this]() -> bool {
    return matchesAny(std::array<std::string, 4>({"*|", "|", "_", "-"}));
  });
}

bool crawler::TokenQueue::matchesWords() {
  return !eof() && isalnum(data.c_str()[pos]);
}

template <size_t N>
bool crawler::TokenQueue::matchesAny(std::array<char, N> seq) {
  if (eof()) {
    return false;
  }
  return std::any_of(seq.cbegin(), seq.cend(),
                     [this](const char c) { return data.c_str()[pos] == c; });
}
template <size_t N>
bool crawler::TokenQueue::matchesAny(std::array<std::string, N> seq) {
  for (auto const &s : seq) {
    if (matches(s)) {
      return true;
    }
  }
  return false;
}
std::string crawler::TokenQueue::chompBalanced(char open, char close) {
  int start = -1;
  int end = -1;
  int depth = 0;
  char last = 0;
  bool isSingleQuote = false;
  bool isDoubleQuote = false;
  do {
    if (eof()) {
      break;
    }
    char c = consume();
    if (last != ESC) {
      if (c == '\'' && c != open && !isDoubleQuote) {
        isSingleQuote = !isSingleQuote;
      } else if (c == '\"' && c != open && !isSingleQuote) {
        isDoubleQuote = !isDoubleQuote;
      }
      if (isSingleQuote || isDoubleQuote) {
        continue;
      }
      if (c == open) {
        depth++;
        if (start == -1) {
          start = pos;
        }
      } else if (c == close) {
        depth--;
      }
    }
    if (depth > 0 && last != 0) {
      // don't include the outer match pair in the return
      end = pos;
    }
    last = c;
  } while (depth > 0);
  const int length = end - start;
  const std::string result = (end >= 0) ? data.substr(start, length) : "";
  if (depth > 0) {
    // throw exception?
  }
  return result;
}

char crawler::TokenQueue::consume() { return data.c_str()[pos++]; }

std::shared_ptr<crawler::Evaluator *> crawler::QueryParser::parse() {
  // not support combinator for now.
  tokenQueue.consumeWhiteSpace();
  findElements();
  while (!tokenQueue.eof()) {
    tokenQueue.consumeWhiteSpace();
    findElements();
  }
  if (evals.size() == 1) {
    return std::make_shared<crawler::Evaluator *>(evals.front());
  } else {
    crawler::And andEvaluator(evals);
    return std::make_shared<crawler::Evaluator *>(&andEvaluator);
  }
}
void crawler::QueryParser::findElements() {
  if (tokenQueue.matchesChomp("#")) {
    findById();
  } else if (tokenQueue.matchesChomp(".")) {
    findByClass();
  } else if (tokenQueue.matchesWords()) {
    findByTag();
  }
}
void crawler::QueryParser::findById() {
  const std::string id = tokenQueue.consumeCssIdentifier();
  evals.emplace_back(new Id(id));
}
void crawler::QueryParser::findByClass() {
  const std::string clazz = tokenQueue.consumeCssIdentifier();
  evals.emplace_back(new Class(clazz));
}

void crawler::QueryParser::findByTag() {
  std::string tagName = tokenQueue.consumeElementSelector();
  evals.emplace_back(new Tag(tagName));
}
crawler::QueryParser::QueryParser(const std::string &queryString)
    : queryString(queryString), tokenQueue(queryString) {}

crawler::Id::Id(std::string id) : id(std::move(id)) {}
bool crawler::Id::matches(const crawler::Node &root,
                          const crawler::Node &node) {
  return id == node.getElementData().id();
}
crawler::Class::Class(std::string clazz) : clazz(std::move(clazz)) {}
bool crawler::Class::matches(const crawler::Node &root,
                             const crawler::Node &node) {
  return clazz == node.getElementData().clazz();
}
crawler::Tag::Tag(std::string tagName) : tagName(std::move(tagName)) {}
bool crawler::Tag::matches(const crawler::Node &root,
                           const crawler::Node &node) {
  return tagName == node.getElementData().getTagName();
}
crawler::CombiningEvaluator::CombiningEvaluator(
    std::vector<Evaluator *> evalutors)
    : evalutors(std::move(evalutors)) {}

bool crawler::And::matches(const crawler::Node &root,
                           const crawler::Node &node) {
  return std::all_of(
      this->evalutors.cbegin(), this->evalutors.cend(),
      [&](auto const &eval) { return eval->matches(root, node); });
}
crawler::And::And(const std::vector<Evaluator *> &evalutors)
    : CombiningEvaluator(evalutors) {}

bool crawler::Or::matches(const crawler::Node &root,
                          const crawler::Node &node) {
  return std::any_of(
      this->evalutors.cbegin(), this->evalutors.cend(),
      [&](auto const &eval) { return eval->matches(root, node); });
}
crawler::Or::Or(const std::vector<Evaluator *> &evalutors)
    : CombiningEvaluator(evalutors) {}
