#pragma once
#include "tgbm/tools/json_tools/generator_parser/basic_parser.hpp"

namespace tgbm::generator_parser {

struct ignore_parser {
  static bool is_value(event_holder& holder) {
    switch (holder.got) {
      case event_holder::string:
      case event_holder::int64:
      case event_holder::uint64:
      case event_holder::double_:
      case event_holder::bool_:
      case event_holder::null:
        return true;
      default:
        return false;
    }
  }

  static dd::generator<nothing_t> parse(event_holder& holder) {
    if (is_value(holder)) {
      co_return;
    }
    std::size_t depth = 0;
    while (true) {
      switch (holder.got) {
        case event_holder::array_begin:
        case event_holder::object_begin:
          depth++;
          co_yield {};
          break;
        case event_holder::part:
          unreachable();
        case event_holder::array_end:
        case event_holder::object_end:
          depth--;
          if (depth == 0)
            co_return;
        case event_holder::string:
        case event_holder::int64:
        case event_holder::uint64:
        case event_holder::double_:
        case event_holder::bool_:
        case event_holder::null:
        case event_holder::key:
          co_yield {};
      }
    }
  }
};
}  // namespace tgbm::generator_parser
