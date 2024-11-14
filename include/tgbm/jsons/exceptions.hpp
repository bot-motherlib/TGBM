#pragma once

#include <stdexcept>

namespace tgbm::json {

struct parse_error : std::runtime_error {
  explicit parse_error(const std::string& s) : std::runtime_error(s) {
  }
  parse_error() : std::runtime_error("Failed parse from json") {
  }
};

struct serialize_error : std::runtime_error {
  explicit serialize_error(const std::string& s) : std::runtime_error(s) {
  }
  serialize_error() : std::runtime_error("Failed serialize from json") {
  }
};

[[noreturn]] inline void throw_json_parse_error() {
  throw parse_error();
}

[[noreturn]] inline void throw_json_serialize_error() {
  throw serialize_error();
}

}  // namespace tgbm::json

#define TGBM_JSON_PARSE_ERROR ::tgbm::json::throw_json_parse_error()
#define TGBM_JSON_SERIALIZE_ERROR ::tgbm::json::throw_json_serialize_error()
