#pragma once

#if 0

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <tgbm/tools/json_parser/aggregate.h>
#include <stdexcept>
#include <type_traits>

namespace tgbm::json {
template <typename T>
  requires std::is_aggregate_v<T>
struct ParserHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ParserHandler<T>> {
  ParserHandler(T& t) : parser(t) {
  }
  bool handle_result(ResultParse res) {
    if (res == ResultParse::kEnd) {
      parsed_ = true;
      return true;
    } else if (res == ResultParse::kContinue) {
      return true;
    } else
      return false;
  }
  bool StartObject() {
    return handle_result(parser.start_object());
  }
  bool EndObject(size_t memberCount) {
    return handle_result(parser.end_object());
  }
  bool StartArray() {
    return handle_result(parser.start_array());
  }
  bool EndArray(size_t elementCount) {
    return handle_result(parser.end_array());
  }
  bool Key(const char* str, rapidjson::SizeType length, bool copy) {
    return handle_result(parser.key(std::string_view{str, length}));
  }
  bool String(const char* str, rapidjson::SizeType length, bool copy) {
    return handle_result(parser.string(std::string_view{str, length}));
  }
  bool Int(int i) {
    return handle_result(parser.integral(i));
  }
  bool Uint(unsigned i) {
    return handle_result(parser.unsigned_integral(i));
  }
  bool Int64(int64_t i) {
    return handle_result(parser.integral(i));
  }
  bool Uint64(uint64_t i) {
    return handle_result(parser.unsigned_integral(i));
  }
  bool Double(double d) {
    return handle_result(parser.floating(d));
  }
  bool Bool(bool b) {
    return handle_result(parser.boolean(b));
  }
  bool Null() {
    return handle_result(parser.null());
  }

 private:
  parser<T> parser;
  bool parsed_ = false;
};

template <typename T>
T from_json(std::string_view sv) {
  std::string_view ss(sv);
  boost::iostreams::stream<boost::iostreams::basic_array_source<char>> stream(sv.data(), sv.size());
  rapidjson::IStreamWrapper isw(stream);
  T t;
  tgbm::json::ParserHandler<T> handler(t);
  rapidjson::Reader reader;
  rapidjson::ParseResult result = reader.Parse(isw, handler);
  if (result.IsError()) {
    throw std::runtime_error(fmt::format("Failed parse json. Offset: {}", result.Offset()));
  }
  return t;
}

}  // namespace tgbm::json

#endif
