#pragma once
#include <stdexcept>
#include "tgbm/logger.h"

namespace tgbm::json {
struct parse_error : std::runtime_error {
  parse_error() : std::runtime_error("Failed parse from json") {
  }
};

struct serialize_error : std::runtime_error {
  serialize_error() : std::runtime_error("Failed serialize from json") {
  }
};

[[noreturn]] inline void throw_json_parse_error() {
  throw parse_error();
}

[[noreturn]] inline void throw_json_serialize_error() {
  LOG("reading to throw TGBM_JSON_SERIALIZE_ERROR");
  throw serialize_error();
}

}  // namespace tgbm::json

#define TGBM_JSON_PARSE_ERROR ::tgbm::json::throw_json_parse_error()
#define TGBM_JSON_SERIALIZE_ERROR ::tgbm::json::throw_json_serialize_error()
