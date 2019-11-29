#include "json.hpp"

#include <utility>

crawler::JsonValue crawler::JsonParser::parse() {
  parseWhitespace();
  parseValue();
  parseWhitespace();
  if (!eof()) {
    throw std::runtime_error("PARSE_ROOT_NOT_SINGULAR");
  }
  return this->jsonValue;
}

void crawler::JsonParser::parseWhitespace() {
  const char *jsonChars = json.c_str();
  while (jsonChars[pos] == ' ' || jsonChars[pos] == '\t' ||
         jsonChars[pos] == '\n' || jsonChars[pos] == '\r') {
    pos++;
  }
}

crawler::JsonParser::JsonParser(std::string json)
    : json(std::move(json)), pos(0), jsonValue(JsonType::_NULL) {}

const std::string &crawler::JsonParser::getJson() const { return json; }

void crawler::JsonParser::parseTrue() {
  parseLiteralness(TRUE, JsonType::TRUE);
}

void crawler::JsonParser::parseFalse() {
  parseLiteralness(FALSE, JsonType::FALSE);
}

void crawler::JsonParser::parseNull() {
  parseLiteralness(_NULL, JsonType::_NULL);
}

void crawler::JsonParser::parseLiteralness(const std::string &literal,
                                           crawler::JsonType jsonType) {
  size_t literalSize = literal.size();
  std::string compareData = json.substr(pos, literalSize);
  if (literal != compareData) {
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
  pos += literalSize;
  this->jsonValue.setType(jsonType);
}

char crawler::JsonParser::currentChar() { return json.c_str()[pos]; }

void crawler::JsonParser::parseValue() {
  switch (currentChar()) {
  case 'n':
    return parseNull();
  case 't':
    return parseTrue();
  case 'f':
    return parseFalse();
  case '\0':
    throw std::runtime_error("PARSE_EXPECT_VALUE");
  default:
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
}

size_t crawler::JsonParser::getPos() const { return pos; }

bool crawler::JsonParser::eof() { return pos >= json.size(); }

crawler::JsonType crawler::JsonValue::getType() const { return type; }

crawler::JsonValue::JsonValue(crawler::JsonType type) : type(type) {}
void crawler::JsonValue::setType(crawler::JsonType _type) {
  JsonValue::type = _type;
}
