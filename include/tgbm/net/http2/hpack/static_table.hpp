#pragma once

#include "tgbm/net/http2/hpack/basic_types.hpp"

namespace tgbm::hpack {

struct static_table_t {
  enum values : uint8_t {
    not_found = 0,
#define STATIC_TABLE_ENTRY(cppname, ...) cppname,
#include "tgbm/net/http2/static_table.def"
    first_unused_index,
  };
  // postcondition: returns < first_unused_index()
  // and 0 ('not_found') when not found
  static index_type find(std::string_view name) noexcept;

  static find_result_t find(std::string_view name, std::string_view value) noexcept;

  // returns 'not_found' if not found
  static index_type find_by_value(std::string_view value) noexcept;

  [[nodiscard]] static find_result_t find(index_type name, std::string_view value) noexcept;

  // precondition: index < first_unused_index && index != 0
  // .value empty if no cached
  static table_entry get_entry(index_type index);
};

}  // namespace tgbm::hpack
