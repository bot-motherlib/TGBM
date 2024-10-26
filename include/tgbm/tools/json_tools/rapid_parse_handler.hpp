#pragma once

#include <stdexcept>

#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include "tgbm/tools/json_tools/handler_parser/basic_parser.hpp"
#include "tgbm/api/common.hpp"

namespace tgbm::json::rapid {

namespace details {

template <typename T>
struct parser_handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, parser_handler<T>> {
  parser_handler(T& t) {
    stack_.push(handler_parser::parser<T>{stack_, t});
  }
  bool StartObject() {
    return stack_.start_object();
  }
  bool EndObject(size_t memberCount) {
    return stack_.end_object();
  }
  bool StartArray() {
    return stack_.start_array();
  }
  bool EndArray(size_t elementCount) {
    return stack_.end_array();
  }
  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    return stack_.key(std::string_view{str, length});
  }
  bool String(const char* str, rapidjson::SizeType length, bool copy) {
    return stack_.string(std::string_view{str, length});
  }
  bool Int(int i) {
    return stack_.integral(i);
  }
  bool Uint(unsigned i) {
    return stack_.unsigned_integral(i);
  }
  bool Int64(int64_t i) {
    return stack_.integral(i);
  }
  bool Uint64(uint64_t i) {
    return stack_.unsigned_integral(i);
  }
  bool Double(double d) {
    return stack_.floating(d);
  }
  bool Bool(bool b) {
    return stack_.boolean(b);
  }
  bool Null() {
    return stack_.null();
  }

 private:
  handler_parser::ParserStack stack_;
};

struct EmptyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, EmptyHandler> {
  EmptyHandler() {
  }
  bool StartObject() {
    return true;
  }
  bool EndObject(size_t memberCount) {
    return true;
  }
  bool StartArray() {
    return true;
  }
  bool EndArray(size_t elementCount) {
    return true;
  }
  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    return true;
  }
  bool String(const char* str, rapidjson::SizeType length, bool copy) {
    return true;
  }
  bool Int(int i) {
    return true;
  }
  bool Uint(unsigned i) {
    return true;
  }
  bool Int64(int64_t i) {
    return true;
  }
  bool Uint64(uint64_t i) {
    return true;
  }
  bool Double(double d) {
    return true;
  }
  bool Bool(bool b) {
    return true;
  }
  bool Null() {
    return true;
  }
};

struct rapid_sv_stream {
  using Ch = char;

  rapid_sv_stream(std::string_view sv) : sv_(sv) {
  }

  Ch Peek() const {
    if (index < sv_.size()) [[likely]] {
      return sv_[index];
    }
    return '\0';
  }
  Ch Take() {
    Ch res = sv_[index];
    index++;
    return res;
  }
  size_t Tell() const {
    return index;
  }

  Ch* PutBegin() {
    assert(false);
    return 0;
  }
  void Put(Ch) {
    assert(false);
  }
  void Flush() {
    assert(false);
  }
  size_t PutEnd(Ch*) {
    assert(false);
    return 0;
  }

  std::string_view sv_;
  std::size_t index = 0;
};

template <typename T>
struct parsed {
  api::optional<T> value;
  rapidjson::ParseErrorCode code = rapidjson::ParseErrorCode::kParseErrorNone;
  std::size_t offset = 0;
  bool ok() {
    return code == rapidjson::ParseErrorCode::kParseErrorNone;
  }
  bool has_error() {
    return !ok();
  }
};

inline std::string_view to_string_view(rapidjson::ParseErrorCode code) {
  switch (code) {
    case rapidjson::kParseErrorNone:
      return "none";
    case rapidjson::kParseErrorDocumentEmpty:
      return "document_empty";
    case rapidjson::kParseErrorDocumentRootNotSingular:
      return "document_root_not_singular";
    case rapidjson::kParseErrorValueInvalid:
      return "value_invalid";
    case rapidjson::kParseErrorObjectMissName:
      return "object_miss_name";
    case rapidjson::kParseErrorObjectMissColon:
      return "object_miss_colon";
    case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket:
      return "object_miss_comma_or_curly_bracket";
    case rapidjson::kParseErrorArrayMissCommaOrSquareBracket:
      return "array_miss_comma_or_square_bracket";
    case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex:
      return "unicode_escape_invalid_hex";
    case rapidjson::kParseErrorStringUnicodeSurrogateInvalid:
      return "string_unicode_surrogate_invalid";
    case rapidjson::kParseErrorStringEscapeInvalid:
      return "string_escape_invalid";
    case rapidjson::kParseErrorStringMissQuotationMark:
      return "string_miss_quoatation_mark";
    case rapidjson::kParseErrorStringInvalidEncoding:
      return "string_invalid_encoding";
    case rapidjson::kParseErrorNumberTooBig:
      return "number_to_big";
    case rapidjson::kParseErrorNumberMissFraction:
      return "number_miss_fraction";
    case rapidjson::kParseErrorNumberMissExponent:
      return "number_miss_exponent";
    case rapidjson::kParseErrorTermination:
      return "termination";
    case rapidjson::kParseErrorUnspecificSyntaxError:
      return "unspecific_syntax_error";
  }
}

}  // namespace details

template <typename T>
T parse_handler(std::string_view sv) {
  T result;
  details::parser_handler<T> handler(result);
  rapidjson::Reader reader;
  details::rapid_sv_stream ss(sv);
  rapidjson::ParseResult parse_result = reader.Parse(ss, handler);
  if (parse_result.IsError()) {
    throw std::runtime_error(fmt::format("Fail parse"));
  }
  return result;
}

inline rapidjson::ParseResult parse_ignore_handler(std::string_view sv) {
  details::rapid_sv_stream ss(sv);
  details::EmptyHandler handler;
  rapidjson::Reader reader;
  return reader.Parse(ss, handler);
}

}  // namespace tgbm::json::rapid
