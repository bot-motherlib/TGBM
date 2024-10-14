#pragma once

#include <algorithm>
#include <cstdint>
#include <memory_resource>
#include <span>
#include <string_view>
#include <vector>
#include <cassert>
#include <charconv>

#include <boost/intrusive/set.hpp>

#include "tgbm/tools/StringTools.h"
#include "tgbm/tools/boost_intrusive.hpp"
#include "tgbm/tools/macro.hpp"
#include "tgbm/net/http2/errors.hpp"
#include "tgbm/tools/algorithm.hpp"

// TODO? lazy headers

namespace tgbm::hpack {

struct sym_info_t {
  uint32_t bits;
  uint8_t bit_count;
};
extern const sym_info_t huffman_table[257];

// uint16_t(-1) if not found
uint16_t huffman_decode_table_find(sym_info_t info);

// integer/string len
using size_type = uint32_t;
// header index
using index_type = uint32_t;
using byte_t = unsigned char;
using In = const byte_t*;

template <typename T>
concept Out = std::output_iterator<T, byte_t>;

template <typename T>
concept protocol_verifier = requires(T& v) {
  { T::handle_protocol_error() };  // must be [[noreturn]]
  { T::handle_size_error() };      // must be [[noreturn]]
  { v.entry_added(std::string_view{}, std::string_view{}) };
};

struct default_protocol_verificator {
  [[noreturn]] static void handle_protocol_error() {
    throw protocol_error{};
  }
  [[noreturn]] static void handle_size_error() {
    throw protocol_error{};
  }
  static void entry_added(std::string_view, std::string_view) {
  }
};

// may be used if you trust to server/endpoint, boosts performance
struct trusted_verificator {
  [[noreturn]] static void handle_protocol_error() {
    unreachable();
  }
  [[noreturn]] static void handle_size_error() {
    unreachable();
  }
  static void entry_added(std::string_view, std::string_view) {
  }
};

// collects all headers which were during connection,
// this statictic may be used to cache headers at start of next connection
struct collect_headers_verificator : default_protocol_verificator {
  std::unordered_multimap<std::string, std::string> headers;

  void entry_added(std::string_view name, std::string_view value) {
    headers.insert({std::string(name), std::string(value)});
  }
};

namespace noexport {

template <typename T>
struct adapted_output_iterator {
  KELCORO_NO_UNIQUE_ADDRESS T base_it;
  mutable byte_t byte = 0;

  using iterator_category = std::output_iterator_tag;
  using value_type = byte_t;
  using difference_type = std::ptrdiff_t;

  constexpr value_type& operator*() const noexcept {
    return byte;
  }
  constexpr adapted_output_iterator& operator++() {
    *base_it = byte;
    ++base_it;
    return *this;
  }
  constexpr adapted_output_iterator operator++(int) {
    auto cpy = *this;
    ++(*this);
    return cpy;
  }
};

template <Out O>
auto adapt_output_iterator(O it) {
  return adapted_output_iterator<O>(it);
}
template <typename T>
auto adapt_output_iterator(adapted_output_iterator<T> it) {
  return it;
}

inline byte_t* adapt_output_iterator(byte_t* ptr) {
  return ptr;
}
inline byte_t* adapt_output_iterator(std::byte* ptr) {
  return reinterpret_cast<byte_t*>(ptr);
}
inline byte_t* adapt_output_iterator(char* ptr) {
  return reinterpret_cast<byte_t*>(ptr);
}

template <typename Original, typename T>
Original unadapt(adapted_output_iterator<T> it) {
  return Original(std::move(it.base_it));
}

template <typename Original>
Original unadapt(byte_t* ptr) {
  static_assert(std::is_pointer_v<Original>);
  return reinterpret_cast<Original>(ptr);
}

}  // namespace noexport

struct table_entry {
  std::string_view name;   // empty if not found
  std::string_view value;  // empty if no

  constexpr explicit operator bool() const noexcept {
    return !name.empty();
  }
  auto operator<=>(const table_entry&) const = default;
};

struct find_result_t {
  // not found by default
  index_type header_name_index = 0;
  bool value_indexed = false;

  // for std::tie
  constexpr std::pair<index_type, bool> pair() const noexcept {
    return {header_name_index, value_indexed};
  }
  constexpr explicit operator bool() const noexcept {
    return header_name_index != 0;
  }
};

struct static_table_t {
  enum values : uint8_t {
    not_found = 0,
#define STATIC_TABLE_ENTRY(cppname, ...) cppname,
#include "tgbm/net/http2/static_table.def"
    first_unused_index,
  };
  // postcondition: returns < first_unused_index()
  // and 0 ('not_found') when not found
  static index_type find(std::string_view name) noexcept {
    return utils::string_switch<index_type>(name)
#define STATIC_TABLE_ENTRY(cppname, name, ...) .case_(name, values::cppname)
#include "tgbm/net/http2/static_table.def"
        .or_default(not_found);
  }

  static find_result_t find(std::string_view name, std::string_view value) noexcept {
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
  static index_type find_by_value(std::string_view value) noexcept {
#define STATIC_TABLE_ENTRY(cppname, name, ...) \
  __VA_OPT__(if (value == std::string_view(__VA_ARGS__)) return values::cppname;)
#include "tgbm/net/http2/static_table.def"
    return not_found;
  }

  [[nodiscard]] static find_result_t find(index_type name, std::string_view value) noexcept {
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
  static table_entry get_entry(index_type index) {
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
};

[[nodiscard]] static bool is_lowercase(std::string_view s) {
  auto is_lower_char = [](char c) { return !(c >= 'A' && c <= 'Z'); };
  return std::find_if_not(s.begin(), s.end(), is_lower_char) == s.end();
}

struct dynamic_table_t {
 private:
  struct entry_t : bi::set_base_hook<bi::link_mode<bi::normal_link>> {
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
  struct key_of_entry {
    using type = table_entry;
    // TODO? try cmp by name + value (one string in entries))?
    table_entry operator()(const entry_t& v) const noexcept {
      return {v.name(), v.value()};
    }
  };

  // invariant: do not contain nullptrs
  std::vector<entry_t*> entries;
  bi::multiset<entry_t, bi::constant_time_size<false>, bi::key_of_value<key_of_entry>> set;
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
                           std::pmr::memory_resource* m = std::pmr::get_default_resource()) noexcept
      : _current_size(0),
        _max_size(max_size),
        _insert_count(0),
        _resource(m ? m : std::pmr::get_default_resource()) {
  }

  dynamic_table_t(const dynamic_table_t&) = delete;

  dynamic_table_t(dynamic_table_t&& other) noexcept
      : entries(std::move(other.entries)),
        set(std::move(other.set)),
        _current_size(std::exchange(other._current_size, 0)),
        _max_size(std::exchange(other._max_size, 0)),
        _insert_count(std::exchange(other._insert_count, 0)),
        _resource(std::exchange(other._resource, std::pmr::get_default_resource())) {
  }

  void operator=(const dynamic_table_t&) = delete;

  dynamic_table_t& operator=(dynamic_table_t&& other) noexcept {
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

  ~dynamic_table_t() {
    reset();
  }

  // returns index of added pair, 0 if cannot add
  index_type add_entry(std::string_view name, std::string_view value) {
    assert(is_lowercase(name));
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

  size_type current_size() const noexcept {
    return _current_size;
  }
  size_type max_size() const noexcept {
    return _max_size;
  }

  void update_size(size_type new_max_size) {
    if (new_max_size > max_size())
      throw protocol_error{};
    evict_until_fits_into(new_max_size);
    _max_size = new_max_size;
  }

  // min value is static_table_t::first_unused_index
  index_type current_max_index() const noexcept {
    return entries.size() + static_table_t::first_unused_index;
  }

  find_result_t find(std::string_view name, std::string_view value) noexcept {
    assert(is_lowercase(name));
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
  find_result_t find(index_type name, std::string_view value) noexcept {
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

  // precondition: first_unused_index <= index <= current_max_index()
  // Note: returned value may be invalidated on next .add_entry()
  table_entry get_entry(index_type index) const noexcept {
    assert(index >= static_table_t::first_unused_index && index <= current_max_index());
    auto& e = *(&entries.back() - (index - static_table_t::first_unused_index));
    return table_entry{e->name(), e->value()};
  }

  static size_type entry_size(const entry_t& entry) noexcept {
    /*
        The size of an entry is the sum of its name's length in octets (as
        defined in Section 5.2), its value's length in octets, and 32.

        entry.name().size() + entry.value().size() + 32;
   */
    return entry.value_end + 32;
  }

  void reset() noexcept {
    set.clear();
    for (entry_t* e : entries)
      entry_t::destroy(e, _resource);
    entries.clear();
    _current_size = 0;
  }
  std::pmr::memory_resource* get_resource() const noexcept {
    return _resource;
  }

 private:
  // precondition: bytes <= _max_size
  void evict_until_fits_into(size_type bytes) noexcept {
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
  // precondition: entry now in 'entries'
  index_type indexof(const entry_t& e) const noexcept {
    return static_table_t::first_unused_index + (_insert_count - e._insert_c);
  }
};

// postcondition: do not overwrites highest 8 - N bits in *out first bytee
// precondition: low N bits of *out first byte is 0
template <std::unsigned_integral UInt = size_type, Out O>
O encode_integer(std::type_identity_t<UInt> I, uint8_t N, O _out) noexcept {
  auto out = noexport::adapt_output_iterator(_out);
  assert(N <= 8 && ((*out & ((1 << N) - 1)) == 0));
  /*
  pseudocode from RFC
   if I < 2^N - 1, encode I on N bits
   else
       encode (2^N - 1) on N bits
       I = I - (2^N - 1)
       while I >= 128
            encode (I % 128 + 128) on 8 bits
            I = I / 128
       encode I on 8 bits
  */
  const uint8_t prefix_max = (1 << N) - 1;
  assert((*out & prefix_max) == 0 && "precondition: low N bits of *out first byte is 0");
  auto push = [&out](uint8_t c) {
    *out = c;
    ++out;
  };
  if (I < prefix_max) {
    // write byte without overwriting existing first 8 - N bits
    *out |= uint8_t(I);
    ++out;
    return noexport::unadapt<O>(out);
  }
  // write byte without overwriting existing first 8 - N bits
  *out |= prefix_max;
  ++out;
  I -= prefix_max;
  while (I >= 128) {
    auto [quot, rem] = div_int(I, 128);
    I = quot;
    push(rem | 0b1000'0000);
  }
  push(I);
  return noexport::unadapt<O>(out);
}

template <protocol_verifier V, std::unsigned_integral UInt = size_type>
[[nodiscard]] size_type decode_integer(In& in, In e, uint8_t N) {
  const UInt prefix_mask = (1 << N) - 1;
  // get first N bits
  auto pull = [&] {
    if (in == e)
      V::handle_size_error();
    int8_t i = *in;
    ++in;
    return i;
  };
  UInt I = pull() & prefix_mask;
  if (I < prefix_mask)
    return I;
  uint8_t M = 0;
  uint8_t B;
  do {
    B = pull();
    UInt cpy = I;
    I += UInt(B & 0b0111'1111) << M;
    if (I < cpy)  // overflow
      V::handle_protocol_error();
    M += 7;
  } while (B & 0b1000'0000);
  return I;
}

template <Out O>
O encode_string_huffman(std::string_view str, O _out) {
  auto out = noexport::adapt_output_iterator(_out);
  // precalculate size
  // (size should be before string and len in bits depends on 'len' value)
  size_type len_after_encode = 0;
  for (char c : str)
    len_after_encode += huffman_table[uint8_t(c)].bit_count;
  *out = 0b1000'0000;  // set H bit
  const int padlen = 8 - len_after_encode % 8;
  out = encode_integer((len_after_encode + padlen) / 8, 7, out);
  auto push_bit = [&, bitn = 7](bool bit) mutable {
    if (bitn == 7)
      *out = 0;
    *out |= (bit << bitn);
    if (bitn == 0) {
      ++out;
      bitn = 7;
      // not set out to 0, because may be end
    } else {
      --bitn;
    }
  };
  for (char c : str) {
    sym_info_t bits = huffman_table[uint8_t(c)];
    for (int i = 0; i < bits.bit_count; ++i)
      push_bit(bits.bits & (1 << i));
  }
  // padding MUST BE formed from EOS la-la-la (just 111..)
  for (int i = 0; i < padlen; ++i)
    push_bit(true);
  return noexport::unadapt<O>(out);
}

// precondition: in != e
template <protocol_verifier V, Out O>
O decode_string_huffman(In& in, In e, O out) {
  assert(in != e && *in & 0b1000'0000);  // Huffman encoded
  size_type str_len = decode_integer<V>(in, e, 7);
  if (str_len > std::distance(in, e))
    V::handle_size_error();
  sym_info_t info{0, 0};
  int bit_nmb = 0;
  auto next_bit = [&] {
    bool bit = *in & (0b1000'0000 >> bit_nmb);
    if (bit_nmb == 7) {
      bit_nmb = 0;
      ++in;
      --str_len;
    } else {
      ++bit_nmb;
    }
    return bit;
  };
  for (;;) {
    // min symbol len in Huffman table is 5 bits
    for (int i = 0; str_len && i < 5; ++i, ++info.bit_count) {
      info.bits <<= 1;
      info.bits += next_bit();
    }
    uint16_t sym;
    while ((sym = huffman_decode_table_find(info)) == uint16_t(-1) && str_len) {
      info.bits <<= 1;
      info.bits += next_bit();
      ++info.bit_count;
    }
    if (sym == 256) [[unlikely]] {
      // TODO? throw STREAM_CLOSED?
      // EOS
      while (bit_nmb != 0)  // skip padding
        next_bit();
      return out;
    }
    if (sym != uint16_t(-1)) {
      info = {};
      *out = byte_t(sym);
      ++out;
    }
    if (!str_len) {
      if (std::countr_one(info.bits) != info.bit_count)
        V::handle_protocol_error();  // incorrect padding
      return out;
    }
  }
  return out;
}

template <bool Huffman = false, Out O>
O encode_string(std::string_view str, O _out) {
  auto out = noexport::adapt_output_iterator(_out);
  /*
       0   1   2   3   4   5   6   7
     +---+---+---+---+---+---+---+---+
     | H |    String Length (7+)     |
     +---+---------------------------+
     |  String Data (Length octets)  |
     +-------------------------------+
  */
  if constexpr (!Huffman) {
    *out = 0;  // set H bit to 0
    out = encode_integer(str.size(), 7, out);
    out = std::copy_n(str.data(), str.size(), out);
  } else {
    out = encode_string_huffman(str, out);
  }
  return noexport::unadapt<O>(out);
}

// since string can be Huffman encoded impossible to use string_view
template <protocol_verifier V, Out O>
O decode_string(In& in, In e, O out) {
  if (*in & 0b1000'0000)  // Huffman encoded
    return decode_string_huffman<V>(in, e, out);
  size_type len = decode_integer<V>(in, e, 7);
  if (std::distance(in, e) < len)
    V::handle_size_error();
  out = std::copy_n(in, len, out);
  std::advance(in, len);
  return out;
}

template <protocol_verifier V>
[[nodiscard]] table_entry get_by_index(index_type header_index, dynamic_table_t& dyntab) {
  /*
     Indices strictly greater than the sum of the lengths of both tables
     MUST be treated as a decoding error.
  */
  if (header_index > dyntab.current_max_index() || header_index == 0)
    V::handle_protocol_error();
  if (header_index < static_table_t::first_unused_index)
    return static_table_t::get_entry(header_index);
  return dyntab.get_entry(header_index);
}

struct encoder {
  dynamic_table_t dyntab;

  // 4096 - default size in HTTP/2
  explicit encoder(size_type max_dyntab_size = 4096,
                   std::pmr::memory_resource* resource = std::pmr::get_default_resource())
      : dyntab(max_dyntab_size, resource) {
  }

  encoder(encoder&&) = default;
  encoder& operator=(encoder&&) noexcept = default;

  // indexed name and value, for example ":path" "/index.html" from static table
  // or some index from dynamic table
  template <Out O>
  O encode_header_fully_indexed(index_type header_index, O _out) {
    /*
          0   1   2   3   4   5   6   7
        +---+---+---+---+---+---+---+---+
        | 1 |        Index (7+)         |
        +---+---------------------------+
    */
    assert(header_index <= dyntab.current_max_index());
    auto out = noexport::adapt_output_iterator(_out);
    // indexed name and value 0b1...
    *out = 0b1000'0000;
    return noexport::unadapt<O>(encode_integer(header_index, 7, out));
  }

  // only name indexed
  // precondition: header_index present in static or dynamic table
  template <bool Huffman = false, Out O>
  O encode_header_and_cache(index_type header_index, std::string_view value, O _out) {
    assert(header_index <= dyntab.current_max_index() && header_index != 0);
    auto out = noexport::adapt_output_iterator(_out);
    // indexed name, new value 0b01...
    *out = 0b0100'0000;
    out = encode_integer(header_index, 6, out);
    // trusted, since im encoder and must not fail
    std::string_view str = get_by_index<trusted_verificator>(header_index, dyntab).name;
    dyntab.add_entry(str, value);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  // indexes value for future use
  // 'out_index' contains index of 'name' + 'value' pair after encode
  template <bool Huffman = false, Out O>
  O encode_header_and_cache(std::string_view name, std::string_view value, O _out) {
    /*
    new name
       0   1   2   3   4   5   6   7
     +---+---+---+---+---+---+---+---+
     | 0 | 1 |           0           |
     +---+---+-----------------------+
     | H |     Name Length (7+)      |
     +---+---------------------------+
     |  Name String (Length octets)  |
     +---+---------------------------+
     | H |     Value Length (7+)     |
     +---+---------------------------+
     | Value String (Length octets)  |
     +-------------------------------+
    */
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0b0100'0000;
    ++out;
    out = encode_string<Huffman>(name, out);
    dyntab.add_entry(name, value);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  template <bool Huffman = false, Out O>
  O encode_header_without_indexing(index_type name, std::string_view value, O _out) {
    /*
        0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
      | 0 | 0 | 0 | 0 |  Index (4+)   |
      +---+---+-----------------------+
      | H |     Value Length (7+)     |
      +---+---------------------------+
      | Value String (Length octets)  |
      +-------------------------------+
    */
    assert(name <= dyntab.current_max_index());
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0;
    out = encode_integer(name, 4, out);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  template <bool Huffman = false, Out O>
  O encode_header_without_indexing(std::string_view name, std::string_view value, O _out) {
    /*
        0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
      | 0 | 0 | 0 | 0 |       0       |
      +---+---+-----------------------+
      | H |     Name Length (7+)      |
      +---+---------------------------+
      |  Name String (Length octets)  |
      +---+---------------------------+
      | H |     Value Length (7+)     |
      +---+---------------------------+
      | Value String (Length octets)  |
      +-------------------------------+
    */
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0;
    ++out;
    out = encode_string<Huffman>(name, out);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  // same as without_indexing, but should not be stored in any proxy memory etc
  template <bool Huffman = false, Out O>
  O encode_header_never_indexing(index_type name, std::string_view value, O _out) {
    /*
        0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
      | 0 | 0 | 0 | 1 |  Index (4+)   |
      +---+---+-----------------------+
      | H |     Value Length (7+)     |
      +---+---------------------------+
      | Value String (Length octets)  |
      +-------------------------------+
    */
    assert(name <= dyntab.current_max_index());
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0b0001'0000;
    out = encode_integer(name, 4, out);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  template <bool Huffman = false, Out O>
  O encode_header_never_indexing(std::string_view name, std::string_view value, O _out) {
    /*
        0   1   2   3   4   5   6   7
      +---+---+---+---+---+---+---+---+
      | 0 | 0 | 0 | 1 |       0       |
      +---+---+-----------------------+
      | H |     Name Length (7+)      |
      +---+---------------------------+
      |  Name String (Length octets)  |
      +---+---------------------------+
      | H |     Value Length (7+)     |
      +---+---------------------------+
      | Value String (Length octets)  |
      +-------------------------------+
    */
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0b0001'0000;
    ++out;
    out = encode_string<Huffman>(name, out);
    return noexport::unadapt<O>(encode_string<Huffman>(value, out));
  }

  /*
   more calculations, less memory
   usually its better to encode headers manually, but may be used as "okay somehow encode"

   'Cache' - if true, then will cache headers if they are not in cache yet
   'Huffman' - use Huffman encoding for strings or no (prefer no

  Note: static table has priority over dynamic table
   (eg indexed name which is indexed in both tables uses index from static,
  same for name + value pairs)
  */
  template <bool Cache = false, bool Huffman = false, Out O>
  O encode_header_memory_effective(std::string_view name, std::string_view value, O out) {
    find_result_t r2 = static_table_t::find(name, value);
    if (r2.value_indexed)
      return encode_header_fully_indexed(r2.header_name_index, out);
    find_result_t r1 = dyntab.find(name, value);
    if (r1.value_indexed)
      return encode_header_fully_indexed(r1.header_name_index, out);
    if (r2) {
      if constexpr (Cache)
        return encode_header_and_cache<Huffman>(r2.header_name_index, value, out);
      else
        return encode_header_without_indexing<Huffman>(r2.header_name_index, value, out);
    }
    if (r1) {
      if constexpr (Cache)
        return encode_header_and_cache<Huffman>(r1.header_name_index, value, out);
      else
        return encode_header_without_indexing<Huffman>(r1.header_name_index, value, out);
    }
    if constexpr (Cache)
      return encode_header_and_cache<Huffman>(name, value, out);
    else
      return encode_header_without_indexing<Huffman>(name, value, out);
  }

  template <bool Cache = false, bool Huffman = false, Out O>
  O encode_header_memory_effective(index_type name, std::string_view value, O out) {
    find_result_t r2 = static_table_t::find(name, value);
    if (r2.value_indexed)
      return encode_header_fully_indexed(r2.header_name_index, out);
    find_result_t r1 = dyntab.find(name, value);
    if (r1.value_indexed)
      return encode_header_fully_indexed(r1.header_name_index, out);
    if (r2) {
      if constexpr (Cache)
        return encode_header_and_cache<Huffman>(r2.header_name_index, value, out);
      else
        return encode_header_without_indexing<Huffman>(r2.header_name_index, value, out);
    }
    if (r1) {
      if constexpr (Cache)
        return encode_header_and_cache<Huffman>(r1.header_name_index, value, out);
      else
        return encode_header_without_indexing<Huffman>(r1.header_name_index, value, out);
    }
    if constexpr (Cache)
      return encode_header_and_cache<Huffman>(name, value, out);
    else
      return encode_header_without_indexing<Huffman>(name, value, out);
  }

  /*
  An encoder can choose to use less capacity than this maximum size
     (see Section 6.3), but the chosen size MUST stay lower than or equal
     to the maximum set by the protocol.

     A change in the maximum size of the dynamic table is signaled via a
     dynamic table size update (see Section 6.3).  This dynamic table size
     update MUST occur at the beginning of the first header block
     following the change to the dynamic table size.  In HTTP/2, this
     follows a settings acknowledgment (see Section 6.5.3 of [HTTP2]).
  */
  template <Out O>
  O encode_dynamic_table_size_update(size_type new_size, O _out) noexcept {
    /*
         0   1   2   3   4   5   6   7
       +---+---+---+---+---+---+---+---+
       | 0 | 0 | 1 |   Max size (5+)   |
       +---+---------------------------+
    */
    auto out = noexport::adapt_output_iterator(_out);
    *out = 0b0010'0000;
    return noexport::unadapt<O>(encode_integer(new_size, 5, out));
  }
};

template <protocol_verifier Verifier = default_protocol_verificator>
struct decoder {
  dynamic_table_t dyntab;
  KELCORO_NO_UNIQUE_ADDRESS Verifier verifier;

  // 4096 - default size in HTTP/2
  explicit decoder(size_type max_dyntab_size = 4096,
                   std::pmr::memory_resource* resource = std::pmr::get_default_resource())
      : dyntab(max_dyntab_size, resource) {
  }

  decoder(decoder&&) = default;
  decoder& operator=(decoder&&) noexcept = default;
  /*
   Note: this function ignores special 'cookie' header case
   https://www.rfc-editor.org/rfc/rfc7540#section-8.1.2.5
   and protocol error if decoded header name is not lowercase
  */
  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header(In& in, In e, O1 name_out, O2 value_out) {
    if (*in & 0b1000'0000)
      return decode_header_fully_indexed(in, e, name_out, value_out);
    if (*in & 0b0100'0000)
      return decode_header_cache(in, e, name_out, value_out);
    if (*in & 0b0010'0000) {
      dyntab.update_size(decode_dynamic_table_size_update(in, e));
      return {name_out, value_out};
    }
    if (*in & 0b0001'0000)
      return decode_header_never_indexing(in, e, name_out, value_out);
    if ((*in & 0b1111'0000) == 0)
      return decode_header_without_indexing(in, e, name_out, value_out);
    verifier.handle_protocol_error();
  }

  // returns status code
  int decode_response_status(In& in, In e) {
    if (*in & 0b1000'0000) {
      // fast path, fully indexed
      auto in_before = in;
      index_type index = decode_integer<Verifier>(in, e, 7);
      switch (index) {
        case static_table_t::status_200:
          return 200;
        case static_table_t::status_204:
          return 204;
        case static_table_t::status_206:
          return 206;
        case static_table_t::status_304:
          return 304;
        case static_table_t::status_400:
          return 400;
        case static_table_t::status_404:
          return 404;
        case static_table_t::status_500:
          return 500;
      }
      in = in_before;
    }
    // first header of response must be required pseudoheader,
    // which is (for response) only one - ":status"
    std::string status;
    std::string code;
    decode_header(in, e, std::back_inserter(status), std::back_inserter(code));
    if (status != ":status" || code.size() != 3)
      verifier.handle_protocol_error();
    int status_code;
    auto [_, err] = std::from_chars(code.data(), code.data() + 3, status_code);
    if (err != std::errc{})
      verifier.handle_protocol_error();
    return status_code;
  }

  // returns requested new size of dynamic table
  size_type decode_dynamic_table_size_update(In& in, In e) noexcept {
    assert(*in & 0b0010'0000 && !(*in & 0b0100'0000) && !(*in & 0b1000'0000));
    return decode_integer<Verifier>(in, e, 5);
  }

 private:
  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header_fully_indexed(In& in, In e, O1 name_out, O2 value_out) {
    assert(*in & 0b1000'0000);
    index_type index = decode_integer<Verifier>(in, e, 7);
    table_entry entry = get_by_index<Verifier>(index, dyntab);
    // only way to get uncached value is from static table,
    // in dynamic table empty header value ("") is a cached header
    if (index < static_table_t::first_unused_index && entry.value.empty())
      verifier.handle_protocol_error();
    name_out = std::copy_n(entry.name.data(), entry.name.size(), name_out);
    value_out = std::copy_n(entry.value.data(), entry.value.size(), value_out);
    return {name_out, value_out};
  }

  // header with incremental indexing
  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header_cache(In& in, In e, O1 name_out, O2 value_out) {
    assert(in != e && *in & 0b0100'0000);
    std::string name;
    std::string value;
    decode_header_impl(in, e, std::back_inserter(name), std::back_inserter(value), 6);
    name_out = std::copy_n(name.data(), name.size(), name_out);
    value_out = std::copy_n(value.data(), value.size(), value_out);
    verifier.entry_added(name, value);
    dyntab.add_entry(name, value);
    return {name_out, value_out};
  }

  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header_without_indexing(In& in, In e, O1 name_out, O2 value_out) {
    assert(in != e && (*in & 0x1111'0000) == 0);
    return decode_header_impl(in, e, name_out, value_out, 4);
  }

  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header_never_indexing(In& in, In e, O1 name_out, O2 value_out) {
    assert(in != e && *in & 0b0001'0000);
    return decode_header_impl(in, e, name_out, value_out, 4);
  }

  // decodes partly indexed / new-name pairs
  template <Out O1, Out O2>
  std::pair<O1, O2> decode_header_impl(In& in, In e, O1 name_out, O2 value_out, uint8_t N) {
    index_type index = decode_integer<Verifier>(in, e, N);
    if (index == 0) {
      name_out = decode_string<Verifier>(in, e, name_out);
    } else {
      table_entry entry = get_by_index<Verifier>(index, dyntab);
      name_out = std::copy_n(entry.name.data(), entry.name.size(), name_out);
    }
    value_out = decode_string<Verifier>(in, e, value_out);
    return {name_out, value_out};
  }
};

template <bool Cache = false, bool Huffman = false, protocol_verifier V, Out O>
O encode_headers_block(encoder& enc, auto&& range_of_headers, O out) {
  for (auto&& [name, value] : range_of_headers)
    out = enc.template encode_header_memory_effective<Cache, Huffman>(name, value, out);
  return out;
}

// visitor should accept two string_views, name and value
// ignores special case Cookie header separated by key-value pairs
template <typename V, typename Verifier>
V decode_headers_block(decoder<Verifier>& enc, std::span<const byte_t> bytes, V visitor) {
  std::vector<byte_t> name;
  std::vector<byte_t> value;
  const auto* in = bytes.data();
  const auto* e = in + bytes.size();
  while (in != e) {
    enc.decode_header(in, e, back_inserter(name), back_inserter(value));
    if (name.empty())
      continue;  // dynamic size update decoded without error
    std::string_view header_name((char*)name.data(), name.size());
    if (!is_lowercase(header_name))
      enc.verifier.handle_protocol_error();
    visitor(header_name, std::string_view((char*)value.data(), value.size()));
    name.clear();
    value.clear();
  }
  return visitor;
}

}  // namespace tgbm::hpack
