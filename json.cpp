#include "json.hpp"

#include <cassert>
#include <exception>
#include <utility>

crawler::JsonValue crawler::JsonParser::parse() {
  parseWhitespace();
  parseValue();
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

void crawler::JsonParser::parseNull() {
  assert(currentChar() == 'n');
  const char *data = json.c_str();
  if (data[pos + 1] != 'u' || data[pos + 2] != 'l' || data[pos + 3] != 'l') {
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
  pos += 4;
  this->jsonValue.setType(JsonType::_NULL);
}

char crawler::JsonParser::currentChar() { return json.c_str()[pos]; }

void crawler::JsonParser::parseValue() {
  switch (currentChar()) {
  case 'n':
    return parseNull();
  case '\0':
    throw std::runtime_error("PARSE_EXPECT_VALUE");
  default:
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
}

size_t crawler::JsonParser::getPos() const { return pos; }

crawler::JsonType crawler::JsonValue::getType() const { return type; }

crawler::JsonValue::JsonValue(crawler::JsonType type) : type(type) {}
void crawler::JsonValue::setType(crawler::JsonType _type) {
  JsonValue::type = _type;
}
