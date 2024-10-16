
#include "tgbm/net/http2/hpack/dynamic_table.hpp"

namespace tgbm::hpack {

struct dynamic_table_t::entry_t : bi::set_base_hook<bi::link_mode<bi::normal_link>> {
  const size_type name_end;
  const size_type value_end;
  const size_t _insert_c;
  char data[];

  entry_t(size_type name_len, size_type value_len, size_t insert_c) noexcept
      : name_end(name_len), value_end(name_len + value_len), _insert_c(insert_c) {
  }

  std::string_view name() const noexcept {
    return {data, data + name_end};
  }
  std::string_view value() const noexcept {
    return {data + name_end, data + value_end};
  }
  size_type size() const noexcept {
    return value_end;
  }

  static entry_t* create(std::string_view name, std::string_view value, size_t insert_c,
                         std::pmr::memory_resource* resource) {
    assert(resource);
    void* bytes = resource->allocate(sizeof(entry_t) + name.size() + value.size(), alignof(entry_t));
    entry_t* e = new (bytes) entry_t(name.size(), value.size(), insert_c);
    std::copy_n(name.data(), name.size(), +e->data);
    std::copy_n(value.data(), value.size(), e->data + name.size());
    return e;
  }
  static void destroy(const entry_t* e, std::pmr::memory_resource* resource) noexcept {
    assert(e && resource);
    std::destroy_at(e);
    resource->deallocate((void*)e, e->value_end, alignof(entry_t));
  }
};

// precondition: 'e' now in entries
index_type dynamic_table_t::indexof(const dynamic_table_t::entry_t& e) const noexcept {
  return static_table_t::first_unused_index + (_insert_count - e._insert_c);
}

static size_type entry_size(const dynamic_table_t::entry_t& entry) noexcept {
  /*
      The size of an entry is the sum of its name's length in octets (as
      defined in Section 5.2), its value's length in octets, and 32.

      entry.name().size() + entry.value().size() + 32;
 */
  return entry.value_end + 32;
}

// TODO? try cmp by name + value (one string in entries))?
table_entry dynamic_table_t::key_of_entry::operator()(const dynamic_table_t::entry_t& v) const noexcept {
  return {v.name(), v.value()};
}

dynamic_table_t::dynamic_table_t(size_type max_size, std::pmr::memory_resource* m) noexcept
    : _current_size(0),
      _max_size(max_size),
      _insert_count(0),
      _resource(m ? m : std::pmr::get_default_resource()) {
}

dynamic_table_t::dynamic_table_t(dynamic_table_t&& other) noexcept
    : entries(std::move(other.entries)),
      set(std::move(other.set)),
      _current_size(std::exchange(other._current_size, 0)),
      _max_size(std::exchange(other._max_size, 0)),
      _insert_count(std::exchange(other._insert_count, 0)),
      _resource(std::exchange(other._resource, std::pmr::get_default_resource())) {
}

dynamic_table_t& dynamic_table_t::operator=(dynamic_table_t&& other) noexcept {
  if (this == &other) [[unlikely]]
    return *this;
  reset();
  entries = std::move(other.entries);
  set = std::move(other.set);
  _current_size = std::exchange(other._current_size, 0);
  _max_size = std::exchange(other._max_size, 0);
  _insert_count = std::exchange(other._insert_count, 0);
  _resource = std::exchange(other._resource, std::pmr::get_default_resource());
  return *this;
}

dynamic_table_t::~dynamic_table_t() {
  reset();
}

// returns index of added pair, 0 if cannot add
index_type dynamic_table_t::add_entry(std::string_view name, std::string_view value) {
  size_type new_entry_size = name.size() + value.size() + 32;
  if (_max_size < new_entry_size) [[unlikely]] {
    reset();
    return 0;
  }
  evict_until_fits_into(_max_size - new_entry_size);
  entries.push_back(entry_t::create(name, value, ++_insert_count, _resource));
  set.insert(*entries.back());
  _current_size += new_entry_size;
  return static_table_t::first_unused_index;
}

void dynamic_table_t::update_size(size_type new_max_size) {
  if (new_max_size > max_size())
    throw protocol_error{};
  evict_until_fits_into(new_max_size);
  _max_size = new_max_size;
}

find_result_t dynamic_table_t::find(std::string_view name, std::string_view value) noexcept {
  find_result_t r;
  auto it = set.find(table_entry(name, value));
  if (it == set.end())
    return r;
  if (name == it->name()) {
    r.header_name_index = indexof(*it);
    if (value == it->value())
      r.value_indexed = true;
  }
  return r;
}
find_result_t dynamic_table_t::find(index_type name, std::string_view value) noexcept {
  assert(name <= current_max_index());
  find_result_t r;
  if (name < static_table_t::first_unused_index || name > current_max_index() || name == 0)
    return r;
  table_entry e = get_entry(name);
  if (e.value == value) {
    r.header_name_index = name;
    r.value_indexed = true;
    return r;
  }
  auto it = set.find(table_entry(e.name, value));
  assert(it != set.end());
  if (e.name == it->name()) {
    r.header_name_index = indexof(*it);
    if (value == it->value())
      r.value_indexed = true;
  }
  return r;
}

void dynamic_table_t::reset() noexcept {
  set.clear();
  for (entry_t* e : entries)
    entry_t::destroy(e, _resource);
  entries.clear();
  _current_size = 0;
}

void dynamic_table_t::evict_until_fits_into(size_type bytes) noexcept {
  assert(bytes <= _max_size);
  size_type i = 0;
  for (; _current_size > bytes; ++i) {
    _current_size -= entry_size(*entries[i]);
    erase_byref(set, *entries[i]);
    entry_t::destroy(entries[i], _resource);
  }
  // evicts should be rare operation
  entries.erase(entries.begin(), entries.begin() + i);
}

table_entry dynamic_table_t::get_entry(index_type index) const noexcept {
  assert(index >= static_table_t::first_unused_index && index <= current_max_index());
  auto& e = *(&entries.back() - (index - static_table_t::first_unused_index));
  return table_entry{e->name(), e->value()};
}

}  // namespace tgbm::hpack
