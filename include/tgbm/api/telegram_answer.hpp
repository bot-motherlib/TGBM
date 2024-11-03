#pragma once

#include "tgbm/api/const_string.hpp"
#include "tgbm/tools/parse_proxy.hpp"

namespace tgbm::api {

// used for telegram answer parsing (tgbm::from_json)
// accepts expected result and parses
// { "ok": <bool>, ?"error_code": <int>?, ?"descripion": <string>?, ?"result": <T>"
template <typename T>
struct telegram_answer {
  bool ok = false;
  parse_proxy<T> result;
  const_string description;
  int error_code = -1;

  explicit telegram_answer(T& out) noexcept : result(out) {
  }
  consteval bool is_optional_field(std::string_view field) {
    return field != "ok";
  }
};

}  // namespace tgbm::api
