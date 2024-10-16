
#include "tgbm/net/http2/hpack/static_table.hpp"

#include "tgbm/tools/StringTools.h"

namespace tgbm::hpack {

// postcondition: returns < first_unused_index()
// and 0 ('not_found') when not found
index_type static_table_t::find(std::string_view name) noexcept {
  return utils::string_switch<index_type>(name)
#define STATIC_TABLE_ENTRY(cppname, name, ...) .case_(name, values::cppname)
#include "tgbm/net/http2/static_table.def"
      .or_default(not_found);
}

find_result_t static_table_t::find(std::string_view name, std::string_view value) noexcept {
  // uses fact, that values in static table are grouped by name
  find_result_t r;
  r.header_name_index = find(name);
  // if no value, than will not find any value anyway
  if (r.header_name_index == not_found)
    return r;
  for (index_type i = r.header_name_index;; ++i) {
    // important: last content entry has no value, so will break loop
    table_entry val = get_entry(i);
    if (val.name != name || val.value.empty())
      return r;
    if (val.value == value) {
      r.header_name_index = i;
      r.value_indexed = true;
      return r;
    }
  }
  return r;
}

// returns 'not_found' if not found
index_type static_table_t::find_by_value(std::string_view value) noexcept {
#define STATIC_TABLE_ENTRY(cppname, name, ...) \
  __VA_OPT__(if (value == std::string_view(__VA_ARGS__)) return values::cppname;)
#include "tgbm/net/http2/static_table.def"
  return not_found;
}

[[nodiscard]] find_result_t static_table_t::find(index_type name, std::string_view value) noexcept {
  find_result_t r;

  auto fill_result = [&]<typename... CharPtrs>(CharPtrs... vars) {
    using case_t = utils::string_switch<bool>::case_t;
    r.value_indexed = (utils::string_switch<bool>(value) | ... | case_t(vars, true)).or_default(false);
    // 'path' + "/" must return 'path'
    // but 'path_index_html' + '/' must return 'path' too
    r.header_name_index = !r.value_indexed ? name : find_by_value(value);
  };
  switch (name) {
    default:
      if (name < first_unused_index && name != not_found)
        r.header_name_index = name;
      return r;
    case method_get:
    case method_post:
      fill_result("GET", "POST");
      return r;
    case path:
    case path_index_html:
      fill_result("/", "/index.html");
      return r;
    case scheme_http:
    case scheme_https:
      fill_result("http", "https");
      return r;
    case status_200:
    case status_204:
    case status_206:
    case status_304:
    case status_400:
    case status_404:
    case status_500:
      fill_result("200", "204", "206", "304", "400", "404", "500");
      return r;
    case accept_encoding:
      fill_result("gzip, deflate");
      return r;
  }
  return r;
}

// precondition: index < first_unused_index && index != 0
// .value empty if no cached
table_entry static_table_t::get_entry(index_type index) {
  assert(index < first_unused_index && index != 0);
  switch (index) {
#define STATIC_TABLE_ENTRY(cppname, name, ...) \
  case values::cppname:                        \
    return {name __VA_OPT__(, __VA_ARGS__)};
#include "tgbm/net/http2/static_table.def"
    default:
      return {"", ""};
  }
}

}  // namespace tgbm::hpack
