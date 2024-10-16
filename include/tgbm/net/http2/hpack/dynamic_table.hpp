#pragma once

#include <memory_resource>

#include <boost/intrusive/set.hpp>

#include "tgbm/tools/boost_intrusive.hpp"
#include "tgbm/net/http2/hpack/basic_types.hpp"
#include "tgbm/net/http2/hpack/static_table.hpp"

namespace tgbm::hpack {

struct dynamic_table_t {
  struct entry_t;

 private:
  struct key_of_entry {
    using type = table_entry;
    table_entry operator()(const entry_t& v) const noexcept;
  };
  // for forward declaring entry_t
  using hook_type_option = bi::base_hook<bi::set_base_hook<bi::link_mode<bi::normal_link>>>;

  // invariant: do not contain nullptrs
  std::vector<entry_t*> entries;
  bi::multiset<entry_t, bi::constant_time_size<false>, hook_type_option, bi::key_of_value<key_of_entry>> set;
  // in bytes
  // invariant: <= _max_size
  size_type _current_size = 0;
  size_type _max_size = 0;
  size_t _insert_count = 0;
  // invariant: != nullptr
  std::pmr::memory_resource* _resource = std::pmr::get_default_resource();
  /*
         <----------  Index Address Space ---------->
         <-- Static  Table -->  <-- Dynamic Table -->
         +---+-----------+---+  +---+-----------+---+
         | 1 |    ...    | s |  |s+1|    ...    |s+k|
         +---+-----------+---+  +---+-----------+---+
                                ^                   |
                                |                   V
                         Insertion Point      Dropping Point
  */
 public:
  dynamic_table_t() = default;
  explicit dynamic_table_t(size_type max_size,
                           std::pmr::memory_resource* m = std::pmr::get_default_resource()) noexcept;

  dynamic_table_t(const dynamic_table_t&) = delete;

  dynamic_table_t(dynamic_table_t&& other) noexcept;

  void operator=(const dynamic_table_t&) = delete;

  dynamic_table_t& operator=(dynamic_table_t&& other) noexcept;

  ~dynamic_table_t();

  // returns index of added pair, 0 if cannot add
  index_type add_entry(std::string_view name, std::string_view value);

  size_type current_size() const noexcept {
    return _current_size;
  }
  size_type max_size() const noexcept {
    return _max_size;
  }

  void update_size(size_type new_max_size);

  // min value is static_table_t::first_unused_index
  index_type current_max_index() const noexcept {
    return entries.size() + static_table_t::first_unused_index;
  }

  find_result_t find(std::string_view name, std::string_view value) noexcept;
  find_result_t find(index_type name, std::string_view value) noexcept;

  // precondition: first_unused_index <= index <= current_max_index()
  // Note: returned value may be invalidated on next .add_entry()
  table_entry get_entry(index_type index) const noexcept;

  void reset() noexcept;
  std::pmr::memory_resource* get_resource() const noexcept {
    return _resource;
  }

 private:
  // precondition: bytes <= _max_size
  void evict_until_fits_into(size_type bytes) noexcept;
  // precondition: entry now in 'entries'
  index_type indexof(const entry_t& e) const noexcept;
};

// searches in both static and dynamic tables
// dyntab is used only if required (index >= 62)
[[nodiscard]] inline table_entry get_by_index(index_type header_index, dynamic_table_t* dyntab) {
  /*
     Indices strictly greater than the sum of the lengths of both tables
     MUST be treated as a decoding error.
  */
  if (header_index == 0) [[unlikely]]
    handle_protocol_error();
  if (header_index < static_table_t::first_unused_index)
    return static_table_t::get_entry(header_index);
  if (header_index > dyntab->current_max_index()) [[unlikely]]
    handle_protocol_error();
  return dyntab->get_entry(header_index);
}

}  // namespace tgbm::hpack
