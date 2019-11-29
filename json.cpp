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

void crawler::JsonParser::parseNumber() {
  const char *data = json.c_str();
  size_t copyPos = pos;
  // negative: -
  if (data[copyPos] == '-') {
    copyPos++;
  }
  // int: "0" / digit1-9 *digit
  if (data[copyPos] == '0') {
    copyPos++;
  } else {
    if (!isDigit1To9(data[copyPos])) {
      throw std::runtime_error("PARSE_INVALID_VALUE");
    }
    for (copyPos++; isdigit(data[copyPos]); copyPos++)
      ;
  }

  // frac: "." 1*digit
  if (data[copyPos] == '.') {
    copyPos++;
    if (!isdigit(data[copyPos])) {
      throw std::runtime_error("PARSE_INVALID_VALUE");
    }
    for (copyPos++; isdigit(data[copyPos]); copyPos++)
      ;
  }

  // exp
  if (data[copyPos] == 'e' || data[copyPos] == 'E') {
    copyPos++;
    if (data[copyPos] == '+' || data[copyPos] == '-') {
      copyPos++;
    }
    if (!isdigit(data[copyPos])) {
      throw std::runtime_error("PARSE_INVALID_VALUE");
    }
    for (copyPos++; isdigit(data[copyPos]); copyPos++)
      ;
  }

  char *endPtr;
  double number = strtod(remainingData().c_str(), &endPtr);
  // if endPtr points end of the string.
  pos = *endPtr == '\0' ? json.size()
                        : crawler::indexOf(std::string(endPtr), json);
  this->jsonValue.setData(number);
  this->jsonValue.setType(JsonType::NUMBER);
}

void crawler::JsonParser::parseValue() {
  switch (currentChar()) {
  case 'n':
    return parseNull();
  case 't':
    return parseTrue();
  case 'f':
    return parseFalse();
  case '0' ... '9':
    return parseNumber();
  case '-':
    return parseNumber();
  case '\0':
    throw std::runtime_error("PARSE_EXPECT_VALUE");
  default:
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
}

char crawler::JsonParser::currentChar() { return json.c_str()[pos]; }

size_t crawler::JsonParser::getPos() const { return pos; }

bool crawler::JsonParser::eof() { return pos >= json.size(); }
std::string crawler::JsonParser::remainingData() {
  return json.substr(pos, json.size() - pos);
}
bool crawler::JsonParser::isDigit1To9(char digit) {
  return digit >= '1' && digit <= '9';
}

crawler::JsonType crawler::JsonValue::getType() const { return type; }

crawler::JsonValue::JsonValue(crawler::JsonType type) : type(type) {}
void crawler::JsonValue::setType(crawler::JsonType _type) {
  JsonValue::type = _type;
}
crawler::JsonValue::JsonValue(crawler::JsonType _jsonType,
                              crawler::JsonData _jsonData) {
  JsonValue::data = _jsonData;
  JsonValue::type = _jsonType;
}

double crawler::JsonValue::getNumber() {
  assert(type == JsonType::NUMBER);
  return std::get<double>(data);
}
void crawler::JsonValue::setData(const crawler::JsonData &_data) {
  JsonValue::data = _data;
}
