#ifndef JOSN_HPP_H__
#define JOSN_HPP_H__
#include "strings.hpp"
#include <cassert>
#include <exception>
#include <string>
#include <variant>

namespace crawler {
enum class JsonType { _NULL, FALSE, TRUE, NUMBER, STRING, ARRAY, OBJECT };
enum class ParseResult {
  PARSE_OK,
  PARSE_EXPECT_VALUE,
  PARSE_INVALID_VALUE,
  PARSE_ROOT_NOT_SINGULAR
};
class Context {
private:
  std::string jsonStr;
};
using JsonData = std::variant<double, bool>;

class JsonValue {
public:
  explicit JsonValue(JsonType type = JsonType::_NULL);

  explicit JsonValue(JsonType jsonType, JsonData jsonData);

  [[nodiscard]] JsonType getType() const;

  void setType(JsonType _type);

  void setData(const JsonData &_data);

  double getNumber();

private:
  JsonData data;

  JsonType type;
};

class JsonParser {
public:
  explicit JsonParser(std::string json);

  // Parse string to JsonValue
  JsonValue parse();

  [[nodiscard]] const std::string &getJson() const;

  [[nodiscard]] size_t getPos() const;

private:
  /// Whitespace ws = *(%x20 / %x09 / %x0A / %x0D)
  void parseWhitespace();

  /// null = "null"
  void parseNull();

  /// Parse "true"
  void parseTrue();

  /// parse literalness, such as `true`, `false`, `null`.
  void parseLiteralness(const std::string &literal, JsonType jsonType);

  /// Parse "false"
  void parseFalse();

  /// Parse number
  void parseNumber();

  /// Parse value
  void parseValue();

  /// Get currentChar
  char currentChar();

  /// Check if end of line.
  bool eof();

  /// num >=1 and num <= 9
  bool isDigit1To9(char digit);

  std::string remainingData();

  std::string json;

  std::size_t pos;

  JsonValue jsonValue;

  inline static std::string TRUE = "true";

  inline static std::string FALSE = "false";

  inline static std::string _NULL = "null";
};

} // namespace crawler
#endif /* JSON_HPP_H__ */
