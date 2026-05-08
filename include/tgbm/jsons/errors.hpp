#pragma once

#include <stdexcept>

namespace tgbm::json {

struct parse_error : std::runtime_error {
  explicit parse_error(const std::string& s) : std::runtime_error(s) {
  }
  explicit parse_error(const char* p) : std::runtime_error(p) {
  }
};

struct serialize_error : std::runtime_error {
  explicit serialize_error(const std::string& s) : std::runtime_error(s) {
  }
  explicit serialize_error(const char* p) : std::runtime_error(p) {
  }
};

}  // namespace tgbm::json
