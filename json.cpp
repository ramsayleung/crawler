#include "json.hpp"

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string>
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
  while (currentChar() == ' ' || currentChar() == '\t' ||
         currentChar() == '\n' || currentChar() == '\r') {
    pos++;
  }
}

crawler::JsonParser::JsonParser(std::string json)
    : jsonValue(JsonType::_NULL), json(std::move(json)), pos(0) {}

const std::string &crawler::JsonParser::getJson() const { return json; }

void crawler::JsonParser::setJson(const std::string &_json){
    JsonParser::json = _json;
}

void crawler::JsonParser::setPos(const size_t _pos){
    JsonParser::pos = _pos;
}

void crawler::JsonParser::parseTrue() {
  parseLiteralness(TRUE, JsonType::BOOLEAN);
  this->jsonValue.setData(true);
}

void crawler::JsonParser::parseFalse() {
  parseLiteralness(FALSE, JsonType::BOOLEAN);
  this->jsonValue.setData(false);
}

void crawler::JsonParser::parseNull() {
  parseLiteralness(_NULL, JsonType::_NULL);
  this->jsonValue.setData(nullptr);
}

void crawler::JsonParser::parseLiteralness(const std::string &literal,
                                           crawler::JsonType jsonType) {
  size_t literalSize = literal.size();
  std::string compareData = json.substr(pos, literalSize);
  if (literal != compareData) {
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
  // move `pos` steps forward
  pos += literalSize;
  this->jsonValue.setType(jsonType);
}

void crawler::JsonParser::parseObject(){
    jsonValue.setType(crawler::JsonType::OBJECT);
    assert(consumeChar() == '{');
    parseWhitespace();
    if(currentChar()=='}'){
        pos++;
        JsonObject emptyValue;
        jsonValue.setData(emptyValue);
        return;
    }
    while (true) {

        // 1. parse key
       if(currentChar() != '\"'){
            throw std::runtime_error("PARSE_OBJECT_MISS_KEY");
       }

        std::string copyData = json.substr(pos, json.length() - pos);
        JsonParser copyDataParser(copyData);
        copyDataParser.parseString();
        std::string key = copyDataParser.getJsonValue().getString();
        pos = copyDataParser.getPos() + pos;

        // 2. parse whitespace colon whitespace
        parseWhitespace();
        if(consumeChar()!=':'){
            throw std::runtime_error("PARSE_OBJECT_MISS_COLON");
        }
        parseWhitespace();

        // 3. parse jsonvalue.
        copyData = json.substr(pos, json.length() - pos);
        copyDataParser.setJson(copyData);
        copyDataParser.setPos(0);
        copyDataParser.parseValue();
        copyDataParser.parseWhitespace();
        if(getJsonValue().isEmptyValue()){
            JsonObject emptyValue;
            jsonValue.setData(emptyValue);
        }
        JsonObject object = getJsonValue().getObject();
        object.insert(std::pair<std::string, JsonValue>(key, copyDataParser.getJsonValue()));
        pos = copyDataParser.getPos() + pos;

        // 4. parse whitespace [comma | right-curly-brace] whitespace
        parseWhitespace();
        if(currentChar()==','){
            pos++;
            parseWhitespace();
        } else if (currentChar() == '}'){
            pos++;
            jsonValue.setData(object);
            break;
        } else{
            throw std::runtime_error("PARSE_MISS_COMMA_OR_CURLY_BRACKET");
        }
    }
}
void crawler::JsonParser::parseArray() {
  jsonValue.setType(crawler::JsonType::ARRAY);
  assert(consumeChar() == '[');
  parseWhitespace();

  // empty array
  if (currentChar() == ']') {
    pos++;
    JsonArray emptyValue;
    jsonValue.setData(emptyValue);
    return;
  }

  // parse value.
  while (true) {
    std::string copyData = json.substr(pos, json.length() - pos);
    JsonParser copyDataParser(copyData);
    copyDataParser.parseWhitespace();
    copyDataParser.parseValue();
    copyDataParser.parseWhitespace();
    if (getJsonValue().isEmptyValue()) {
      JsonArray emptyValue;
      jsonValue.setData(emptyValue);
    }
    JsonArray array = getJsonValue().getArray();
    array.emplace_back(copyDataParser.getJsonValue());
    jsonValue.setData(array);
    pos = copyDataParser.getPos() + pos;
    if (currentChar() == ',') {
      pos++;
      parseWhitespace();
    } else if (currentChar() == ']') {
      pos++;
      break;
    }
  }
}

void crawler::JsonParser::parseString() {
  // beginning quotation mark.
  assert(consumeChar() == '\"');
  std::string buffer;
  while (true) {
    char ch = consumeChar();
    switch (ch) {
      // ending quotation mark
    case '\"':
      jsonValue.setData(buffer);
      jsonValue.setType(crawler::JsonType::STRING);
      return;
      /* handle escape char */
    case '\\':
      switch (consumeChar()) {
      case '\"':
        buffer += '\"';
        break;
      case '\\':
        buffer += '\\';
        break;
      case '/':
        buffer += '/';
        break;
      case 'b':
        buffer += '\b';
        break;
      case 'f':
        buffer += '\f';
        break;
      case 'n':
        buffer += '\n';
        break;
      case 'r':
        buffer += '\r';
        break;
      case 't':
        buffer += '\t';
        break;
      default:
        throw std::runtime_error("PARSE_INVALID_STRING_ESCAPE");
      }
      break;
    case '\0':
      throw std::runtime_error("PARSE_MISS_QUOTATION_MARK");
    default:
      // handle illegal unescaped char
      if ((unsigned char)ch < 0x20) {
        throw std::runtime_error("PARSE_INVALID_STRING_CHAR");
      }
      buffer += ch;
    }
  }
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
  case '\"':
    return parseString();
  case '[':
    return parseArray();
  case '{':
      return parseObject();
  case '\0':
    throw std::runtime_error("PARSE_EXPECT_VALUE");
  default:
    throw std::runtime_error("PARSE_INVALID_VALUE");
  }
}

char crawler::JsonParser::currentChar() { return json.c_str()[pos]; }

char crawler::JsonParser::consumeChar() { return json.c_str()[pos++]; }

size_t crawler::JsonParser::getPos() const { return pos; }

crawler::JsonValue crawler::JsonParser::getJsonValue() const {
  return jsonValue;
}

bool crawler::JsonParser::eof() { return pos >= json.size(); }
std::string crawler::JsonParser::remainingData() {
  return json.substr(pos, json.size() - pos);
}
bool crawler::JsonParser::isDigit1To9(char digit) {
  return digit >= '1' && digit <= '9';
}

crawler::JsonType crawler::JsonValue::getType() const { return type; }

crawler::JsonValue::JsonValue(crawler::JsonType type)
    : data(nullptr), type(type) {}
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

std::string crawler::JsonValue::getString() {
  assert(type == JsonType::STRING);
  return std::get<std::string>(data);
}

bool crawler::JsonValue::getBoolean() {
  assert(type == JsonType::BOOLEAN);
  return std::get<bool>(data);
}

std::nullptr_t crawler::JsonValue::getNull() {
  assert(type == JsonType::_NULL);
  return std::get<std::nullptr_t>(data);
}

crawler::JsonArray crawler::JsonValue::getArray() {
  assert(type == JsonType::ARRAY);
  return std::get<crawler::JsonArray>(data);
}

crawler::JsonObject crawler::JsonValue::getObject(){
    assert(type == JsonType::OBJECT);
    return std::get<crawler::JsonObject>(data);
}

bool crawler::JsonValue::isEmptyValue() {
  try {
    std::get<std::nullptr_t>(data);
    // if doesn't throw exception, return true.
    return true;
  } catch (const std::bad_variant_access &) {
    return false;
  }
}

void crawler::JsonValue::setData(const crawler::JsonData &_data) {
  JsonValue::data = _data;
}
