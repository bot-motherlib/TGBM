#pragma once

namespace tgbm::json::parse_dom {

template <typename T>
struct parser {
  template <typename Json, typename Traits>
  static void parse(const Json&, T&) {
    static_assert(sizeof(Json) == 0, "Write your specialization");
  }
};
}  // namespace tgbm::json::parse_dom
