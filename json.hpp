#ifndef JOSN_HPP_H__
#define JOSN_HPP_H__
#include <string>
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
class JsonValue {
public:
  explicit JsonValue(JsonType type = JsonType::_NULL);
  [[nodiscard]] JsonType getType() const;

  void setType(JsonType _type);

private:
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
  /// ws = *(%x20 / %x09 / %x0A / %x0D)
  void parseWhitespace();

  /// null = "null"
  void parseNull();

  /// parse value
  void parseValue();

  /// Get currentChar
  char currentChar();

  JsonValue jsonValue;

  std::string json;

  std::size_t pos;
};

} // namespace crawler
#endif /* JSON_HPP_H__ */
