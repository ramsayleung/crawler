#ifndef JOSN_HPP_H__
#define JOSN_HPP_H__
#include "strings.hpp"
#include <cassert>
#include <cstddef>
#include <exception>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace crawler {
class JsonValue;
enum class JsonType { _NULL, BOOLEAN, NUMBER, STRING, ARRAY, OBJECT };
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

using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;
using JsonData = std::variant<double, bool, std::nullptr_t, std::string,
                              JsonArray, JsonObject>;

class JsonValue {
public:
  explicit JsonValue(JsonType type = JsonType::_NULL);

  explicit JsonValue(JsonType jsonType, JsonData jsonData);

  [[nodiscard]] JsonType getType() const;

  void setType(JsonType _type);

  double getNumber();

  std::string getString();

  bool getBoolean();

  std::nullptr_t getNull();

  JsonArray getArray();

  JsonObject getObject();

  bool isEmptyValue();

private:
  JsonData data;

public:
  void setData(const JsonData &_data);

private:
  JsonType type;
};

class JsonParser {
public:
  explicit JsonParser(std::string json);

  // Parse string to JsonValue
  JsonValue parse();

  void setJson(const std::string& _json);

  void setPos(const size_t _pos);

  [[nodiscard]] const std::string &getJson() const;

  [[nodiscard]] size_t getPos() const;

  [[nodiscard]] JsonValue getJsonValue() const;

private:
  /// Parse value
  void parseValue();

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
  /// number format:
  /// number = [ "-" ] int [ frac ] [ exp ]
  /// int = "0" / digit1-9 *digit
  /// frac = "." 1*digit
  /// exp = ("e" / "E") ["-" / "+"] 1*digit
  void parseNumber();

  /// Parse string
  // string format:
  // string = quotation-mark *char quotation-mark
  // char = unescaped /
  //    escape (
  //        %x22 /          ; "    quotation mark  U+0022
  //        %x5C /          ; \    reverse solidus U+005C
  //        %x2F /          ; /    solidus         U+002F
  //        %x62 /          ; b    backspace       U+0008
  //        %x66 /          ; f    form feed       U+000C
  //        %x6E /          ; n    line feed       U+000A
  //        %x72 /          ; r    carriage return U+000D
  //        %x74 /          ; t    tab             U+0009
  //        %x75 4HEXDIG )  ; uXXXX                U+XXXX
  // escape = %x5C          ; \
// quotation-mark = %x22  ; "
  // unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
  void parseString();

  /// Parse Array, the array format:
  /// array = %x5B ws [ value *( ws %x2C ws value ) ] ws %x5D
  void parseArray();

  /// Paser object, the object format:
  /// member = string ws %x3A ws value
  /// object = %x7B ws [ member *( ws %x2C ws member ) ] ws %x7D
  void parseObject();

  /// Return current Char
  char currentChar();

  /// Return current char, and then move one step forward.
  char consumeChar();

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
